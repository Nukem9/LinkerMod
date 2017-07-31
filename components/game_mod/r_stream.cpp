#include "stdafx.h"

#define IMAGE_BIT_IS_SET(var, bits) ((var)[BIT_INDEX_32(bits)] & BIT_MASK_32(bits))
#define IMAGE_BIT_SET(var, bits)	((var)[BIT_INDEX_32(bits)] |= BIT_MASK_32(bits));
#define IMAGE_BIT_UNSET(var, bits)	((var)[BIT_INDEX_32(bits)] &= ~BIT_MASK_32(bits));

GfxWorld*& rgp_world = *(GfxWorld **)0x0396A24C;
GfxWorldDpvsStatic*& g_worldDpvs = *(GfxWorldDpvsStatic **)0x0460C0AC;

StreamFrontendGlob streamFrontendGlob;
pendingRequest s_pendingRequests[STREAM_MAX_REQUESTS];

DObj *s_forcedLoadEntities[4];
int s_numForcedLoadEntities;

streamerHintInfo s_streamHints[4];
int s_numStreamHintsActive;

Material *s_preventMaterials[32];

bool streamIsInitialized;
volatile long disableCount;

alignas(16) float4 s_viewPos;

// /gfx_d3d/r_stream.cpp:266
bool R_StreamIsEnabled()
{
	return disableCount == 0;
}

// /gfx_d3d/r_stream.cpp:271
void R_StreamPushDisable()
{
	InterlockedIncrement(&disableCount);
}

// /gfx_d3d/r_stream.cpp:276
void R_StreamPopDisable()
{
	int value = InterlockedDecrement(&disableCount);

	ASSERT_MSG(value >= 0, "disableCount >= 0");
}

// /gfx_d3d/r_stream.cpp:283
void R_StreamSetInitData(int buffer_size)
{
	streamFrontendGlob.mainBufferSize = buffer_size;
}

// /gfx_d3d/r_stream.cpp:288
void R_StreamSetDefaultConfig(bool clear)
{
	streamFrontendGlob.forceDiskOrder = false;
	streamFrontendGlob.touchedImageImportance = FLT_MAX / 2.0f;
	streamFrontendGlob.initialImageImportance = FLT_MAX / 4.0f;
	streamFrontendGlob.forcedImageImportance = FLT_MAX / 2.0f;

	if (clear)
	{
		memset(streamFrontendGlob.imageInitialBits, 0, sizeof(streamFrontendGlob.imageInitialBits));
		memset(streamFrontendGlob.imageForceBits, 0, sizeof(streamFrontendGlob.imageForceBits));

		streamFrontendGlob.imageInitialBitsSet = false;
		streamFrontendGlob.initialLoadAllocFailures = 0;
		streamFrontendGlob.preloadCancelled = false;
	}

	streamFrontendGlob.diskOrderImagesNeedSorting = true;
}

// /gfx_d3d/r_stream.cpp:308
void R_StreamSetUIConfig(bool clear)
{
	R_StreamSetDefaultConfig(clear);

	streamFrontendGlob.forceDiskOrder = true;
	streamFrontendGlob.touchedImageImportance = FLT_MAX / 2.0f;
	streamFrontendGlob.initialImageImportance = FLT_MAX / 4.0f;
	streamFrontendGlob.forcedImageImportance = 0.00001f;
}

// /gfx_d3d/r_stream.cpp:366
float R_Stream_GetProgress()
{
	if (!com_waitForStreamer->current.integer)
		return 1.0f;

	if (streamFrontendGlob.initialLoadAllocFailures > 0)
		return 1.0f;

	if (streamFrontendGlob.imageInitialBitsSet)
	{
		int total = 0;
		int complete = 0;

		for (int idx = 0; idx < STREAM_MAX_IMAGE_BITS; idx++)
		{
			unsigned int bits = streamFrontendGlob.imageInitialBits[idx] | streamFrontendGlob.imageForceBits[idx];

			total += CountBitsEnabled(bits);
			complete += CountBitsEnabled(streamFrontendGlob.imageUseBits[idx] & bits);
		}

		// If (any images were pending)
		if (total > 0)
			return (float)complete / (float)total + 0.001f;

		// No images waiting, so it's considered complete
		return 1.0f;
	}

	return 0.0f;
}

// /gfx_d3d/r_stream.cpp:734
void R_Stream_InvalidateRequest(pendingRequest *request)
{
	memset(request, 0, sizeof(pendingRequest));
	request->status = STREAM_STATUS_INVALID;
}

// /gfx_d3d/r_stream.cpp:746
stream_status R_StreamRequestImageAllocation(pendingRequest *request, GfxImage *image, bool highMip, float importance)
{
	ASSERT(image);
	ASSERT(request);
	ASSERT(image->streaming != GFX_TEMP_STREAMING);
	ASSERT(image->streaming != GFX_NOT_STREAMING);

	if (image->streaming == GFX_NOT_STREAMING)
		return STREAM_STATUS_EOF;

	ASSERT(request->image == nullptr);
	ASSERT(request->status == STREAM_STATUS_INVALID);

	request->image = image;
	request->imagePart = 0;
	request->importance = importance;
	request->startTime = Sys_Milliseconds();
	request->bufferSize = 0;
	request->buffer = nullptr;
	request->highMip = highMip;

	if (r_streamLog && r_streamLog->current.integer > 0)
	{
		Com_PrintMessage(
			16,
			va(
				"-STREAM-allocation complete. bytes=%d,image=%s,importance=%f\n",
				request->bufferSize,
				image->name,
				importance),
			0);
	}

	int imageIndex = DB_GetImageIndex(image);

	ASSERT(!IMAGE_BIT_IS_SET(streamFrontendGlob.imageLoading, imageIndex));

	IMAGE_BIT_SET(streamFrontendGlob.imageLoading, imageIndex);
	request->status = STREAM_STATUS_PRE;
	request->id[0] = -1;
	return STREAM_STATUS_INPROGRESS;
}

// /gfx_d3d/r_stream.cpp:813
void R_StreamUpdate_ReadTextures()
{
	static int lastIdx = 0;

	R_StreamAlloc_Lock();
	pendingRequest *request = nullptr;
	int numRequests = 0;

	for (int i = 0; i < STREAM_MAX_REQUESTS; i++)
	{
		int idx = (lastIdx + i) % STREAM_MAX_REQUESTS;

		if (s_pendingRequests[idx].status != STREAM_STATUS_QUEUED)
			continue;

		if (!request)
		{
			request = &s_pendingRequests[idx];
			request->status = STREAM_STATUS_INPROGRESS;
			lastIdx = (idx + 1) % STREAM_MAX_REQUESTS;
		}

		numRequests++;
	}

	// Create a temporary struct copy of the image (reason unknown)
	GfxImage tempImage;
	bool tempHighMip = false;

	if (request)
	{
		tempHighMip = request->highMip;
		memcpy(&tempImage, request->image, sizeof(GfxImage));
	}

	// Allow other threads to use the global request array
	R_StreamAlloc_Unlock();

	if (request)
	{
		char *tempBuffer;
		int tempBufferSize;

		bool status = Image_LoadToBuffer(&tempImage, tempHighMip, &tempBuffer, &tempBufferSize);

		// Image done loading, so lock the array again
		R_StreamAlloc_Lock();
		{
			if (request->status == STREAM_STATUS_INPROGRESS)
			{
				if (status)
				{
					// Now copy the temporary struct back over
					request->status = STREAM_STATUS_FINISHED;
					request->buffer = tempBuffer;
					request->bufferSize = tempBufferSize;
					memcpy(request->image, &tempImage, sizeof(GfxImage));
				}
				else
				{
					request->status = STREAM_STATUS_CANCELLED;
				}
			}
			else
			{
				Z_VirtualFree(tempBuffer, 20);
			}
		}
		R_StreamAlloc_Unlock();

		Sleep(1);
	}

	// Tell Stream_Thread if there are more requests pending
	if (numRequests > 1)
		Sys_WakeStream();
}

// /gfx_d3d/r_stream.cpp:856
bool R_StreamRequestImageRead(pendingRequest *request)
{
	GfxImage *unloadImage = nullptr;
	GfxImage *image = request->image;

	int imageIndex = DB_GetImageIndex(image);
	request->status = STREAM_STATUS_QUEUED;

	if (!request->highMip)
	{
		Sys_WakeStream();
		return true;
	}

	if (R_StreamAlloc_CanAllocate(image->baseSize, request->importance, &unloadImage) && unloadImage)
	{
		// Find the next free pending request entry
		pendingRequest *unloadRequest = nullptr;

		for (int i = 0; i < STREAM_MAX_REQUESTS; i++)
		{
			if (s_pendingRequests[i].status == STREAM_STATUS_INVALID)
			{
				unloadRequest = &s_pendingRequests[i];
				break;
			}
		}

		// Queue the request to read the image
		if (unloadRequest)
		{
			int unloadImagePartIndex = DB_GetImageIndex(unloadImage);

			ASSERT(IMAGE_BIT_IS_SET(streamFrontendGlob.imageUseBits, unloadImagePartIndex));

			if (R_StreamRequestImageAllocation(
				unloadRequest,
				unloadImage,
				false,
				streamFrontendGlob.imageImportance[unloadImagePartIndex])
				&& R_StreamRequestImageRead(unloadRequest))
			{
				unloadImage = nullptr;
			}
		}
	}

	if (!unloadImage)
	{
		Sys_WakeStream();
		return true;
	}

	IMAGE_BIT_UNSET(streamFrontendGlob.imageLoading, imageIndex);
	R_Stream_InvalidateRequest(request);
	return false;
}

// /gfx_d3d/r_stream.cpp:1127
float PointDistSqFromBounds(const float *v, const float *mins, const float *maxs)
{
	float distBelowMin;
	float distAboveMax;

	distBelowMin = mins[0] - v[0];
	distAboveMax = v[0] - maxs[0];

	double xDist = max(distBelowMin, 0.0) + max(distAboveMax, 0.0);

	distBelowMin = mins[1] - v[1];
	distAboveMax = v[1] - maxs[1];

	double yDist = max(distBelowMin, 0.0) + max(distAboveMax, 0.0);

	distBelowMin = mins[2] - v[2];
	distAboveMax = v[2] - maxs[2];

	double zDist = max(distBelowMin, 0.0) + max(distAboveMax, 0.0);

	return (float)((xDist * xDist) + (yDist * yDist) + (zDist * zDist));
}

// /gfx_d3d/r_stream.cpp:1169
bool R_StreamUpdate_ProcessFileCallbacks()
{
	bool workDone = false;

	for (int i = 0; i < STREAM_MAX_REQUESTS; i++)
	{
		pendingRequest *request = &s_pendingRequests[i];

		// Skip invalid statuses for this function
		if (request->status == STREAM_STATUS_INVALID ||
			request->status == STREAM_STATUS_PRE ||
			request->status == STREAM_STATUS_QUEUED ||
			request->status == STREAM_STATUS_INPROGRESS)
			continue;

		GfxImage *image = request->image;

		ASSERT(image != nullptr);

		int imageIndex = DB_GetImageIndex(image);

		ASSERT(IMAGE_BIT_IS_SET(streamFrontendGlob.imageLoading, imageIndex));

		// Unset the imageLoading bit when the request is 'finished'
		if (request->status == STREAM_STATUS_CANCELLED ||
			request->status == STREAM_STATUS_READFAILED ||
			request->status == STREAM_STATUS_EOF)
		{
			IMAGE_BIT_UNSET(streamFrontendGlob.imageLoading, imageIndex);

			Z_VirtualFree(request->buffer, 20);
			R_Stream_InvalidateRequest(request);
			continue;
		}

		workDone = true;

		ASSERT(request->status == STREAM_STATUS_FINISHED);

		if (Sys_IsRenderThread())
		{
			GfxImage temp;
			memcpy(&temp, image, sizeof(temp));

			image->texture.basemap->AddRef();
			Image_Release(image);

			ASSERT(!image->texture.basemap);

			GfxImageFileHeader *fileHeader = (GfxImageFileHeader *)request->buffer;
			char *imageData = request->buffer + sizeof(GfxImageFileHeader);

			Image_LoadFromData(image, fileHeader, imageData, 2);
			Z_VirtualFree(fileHeader, 20);

			if (!image->texture.basemap)
			{
				Image_Release(image);
				memcpy(image, &temp, sizeof(temp));
			}
			else
			{
				temp.texture.basemap->Release();
			}
		}
		else
		{
			((void(__cdecl *)(GfxImage *, char *))0x006FC1E0)(image, request->buffer);
			// RB_Resource_ReloadTexture(image, request->buffer);
		}

		if (request->highMip)
		{
			ASSERT(!IMAGE_BIT_IS_SET(streamFrontendGlob.imageUseBits, imageIndex));
			IMAGE_BIT_SET(streamFrontendGlob.imageUseBits, imageIndex);
		}
		else
		{
			ASSERT(IMAGE_BIT_IS_SET(streamFrontendGlob.imageUseBits, imageIndex));
			IMAGE_BIT_UNSET(streamFrontendGlob.imageUseBits, imageIndex);
		}

		IMAGE_BIT_UNSET(streamFrontendGlob.imageLoading, imageIndex);
		R_Stream_InvalidateRequest(request);
	}

	return workDone;
}

// /gfx_d3d/r_stream.cpp:1409
void R_StreamUpdate_SetupInitialImageList()
{
	R_StreamSetDefaultConfig(true);

	if (r_streamLowDetail && r_streamLowDetail->current.enabled)
		R_Stream_ForceLoadLowDetail();
	else
		memset(streamFrontendGlob.imageForceBits, 0, sizeof(streamFrontendGlob.imageForceBits));

	streamFrontendGlob.diskOrderImagesNeedSorting = true;
	streamFrontendGlob.imageInitialBitsSet = true;
	streamFrontendGlob.initialLoadAllocFailures = 0;
	streamFrontendGlob.preloadCancelled = true;
}

// /gfx_d3d/r_stream.cpp:???
void R_Stream_ForceLoadLowDetail()
{
	// Code removed completely
}

// /gfx_d3d/r_stream.cpp:1463
void R_StreamUpdate_AddInitialImages(float importance)
{
	for (int imagePartIndex = 0; imagePartIndex < STREAM_MAX_IMAGES; imagePartIndex++)
	{
		if (IMAGE_BIT_IS_SET(streamFrontendGlob.imageInitialBits, imagePartIndex))
			R_Stream_AddImagePartImportance(imagePartIndex, importance);
	}
}

// /gfx_d3d/r_stream.cpp:1474
void R_StreamUpdate_AddForcedImages(float forceImportance, float touchImportance)
{
	// Toggle between index 1 and 0
	streamFrontendGlob.activeImageTouchBits ^= 1;

	for (int index = 0; index < STREAM_MAX_IMAGE_BITS; index++)
	{
		unsigned int touchBits = streamFrontendGlob.imageTouchBits[index][0] | streamFrontendGlob.imageTouchBits[index][1];
		unsigned int forceBits = streamFrontendGlob.imageForceBits[index] | touchBits;
		unsigned int useBits = streamFrontendGlob.imageUseBits[index] & ~forceBits;

		for (int bitIndex = NextMSBSet(forceBits); bitIndex >= 0; bitIndex = NextMSBSet(forceBits))
		{
			const int mask = BIT_MASK_32(bitIndex);

			ASSERT(forceBits & mask);
			forceBits &= ~mask;

			int part;
			part = (bitIndex + 32 * index) & 0x80000000;
			part = max(part, 0) + 1;

			if (mask & touchBits)
				R_Stream_AddImagePartImportance(bitIndex + 32 * index, touchImportance / (float)part);
			else
				R_Stream_AddImagePartImportance(bitIndex + 32 * index, forceImportance / (float)part);
		}

		for (int bitIndex = NextMSBSet(useBits); bitIndex >= 0; bitIndex = NextMSBSet(useBits))
		{
			const int mask = BIT_MASK_32(bitIndex);

			ASSERT(useBits & mask);
			useBits &= ~mask;

			R_Stream_AddImagePartImportance(bitIndex + 32 * index, 0.0f);
		}
	}

	memset(&streamFrontendGlob.imageTouchBits[0][streamFrontendGlob.activeImageTouchBits ^ 1], 0, STREAM_MAX_IMAGE_BITS * sizeof(unsigned int));
}

// /gfx_d3d/r_stream.cpp:1583
void R_Stream_ForceLoadImage(GfxImage *image, int part)
{
	if (!image || image->streaming == GFX_NOT_STREAMING)
		return;

	int imageIndex = DB_GetImageIndex(image);

	if (part)
	{
		if (part >= 0)
		{
			for (int partIndex = 0; partIndex < part && partIndex < STREAM_MAX_IMAGES; partIndex++)
				IMAGE_BIT_SET(streamFrontendGlob.imageForceBits, partIndex + imageIndex);
		}
		else
		{
			int partIndex = (part + 1 < 0) ? 0 : part + 1;

			for (; partIndex >= 0; partIndex--)
				IMAGE_BIT_SET(streamFrontendGlob.imageForceBits, partIndex + imageIndex);
		}
	}
	else
	{
		for (int partIndex = 0; partIndex < STREAM_MAX_IMAGES; partIndex++)
			IMAGE_BIT_UNSET(streamFrontendGlob.imageForceBits, partIndex + imageIndex);
	}

	streamFrontendGlob.diskOrderImagesNeedSorting = true;
}

// /gfx_d3d/r_stream.cpp:1631
void R_Stream_ForceLoadMaterial(Material *material, int part)
{
	if (!material)
		return;

	int textureCount = material->textureCount;

	for (int textureIter = 0; textureIter != textureCount; textureIter++)
	{
		MaterialTextureDef *texDef = &material->textureTable[textureIter];

		ASSERT(texDef);

		if (texDef->semantic == TS_WATER_MAP)
			continue;

		ASSERT(texDef->u.image);
		R_Stream_ForceLoadImage(texDef->u.image, part);
	}
}

// /gfx_d3d/r_stream.cpp:1657
void R_Stream_ForceLoadModel(XModel *model, int part)
{
	if (!model)
		return;

	for (int lod = 0; lod < MAX_LODS; lod++)
	{
		XSurface *surfaces = nullptr;
		Material * const *material = nullptr;

		int surfCount = XModelGetSurfaces(model, &surfaces, lod);
		material = XModelGetSkins(model, lod);

		for (int surfIter = 0; surfIter != surfCount;)
		{
			R_Stream_ForceLoadMaterial(*material, part);
			surfIter++;
			material++;
		}
	}
}

// /gfx_d3d/r_stream.cpp:1684
bool R_StreamTouchDObjAndCheck(DObj *obj, int level)
{
	ASSERT(obj);
	ASSERT(Sys_IsMainThread());
	ASSERT(useFastFile->current.enabled);

	bool streamed = true;
	int boneIndex = 0;
	int modelCount = DObjGetNumModels(obj);

	ASSERT(modelCount <= DOBJ_MAX_SUBMODELS);

	int surfPartBits[9];
	surfPartBits[0] = 0;
	surfPartBits[1] = 0;
	surfPartBits[2] = 0;
	surfPartBits[3] = 0;
	surfPartBits[4] = 0;
	surfPartBits[5] = 0;
	surfPartBits[6] = 0;
	surfPartBits[7] = 0;
	surfPartBits[8] = 0;

	for (int modelIndex = 0; modelIndex < modelCount;)
	{
		XModel *model = DObjGetModel(obj, modelIndex);

		ASSERT(model);

		Material * const *materials = XModelGetSkins(model, 0);
		int boneCount = XModelNumBones(model);

		XSurface *surfaces;
		int surfaceCount = XModelGetSurfaces(model, &surfaces, 0);

		ASSERT(surfaces);
		ASSERT(surfaceCount);

		unsigned int targBoneIndexHigh = boneIndex >> 5;
		unsigned int targBoneIndexLow = boneIndex & 0x1F;
		unsigned int invTargBoneIndexLow = 32 - (boneIndex & 0x1F);

		unsigned int partBits[5];
		unsigned int hidePartBits[5];
		DObjGetHidePartBits(obj, hidePartBits);

		ASSERT(surfaceCount <= DOBJ_MAX_SURFS);

		for (int surfaceIndex = 0; surfaceIndex < surfaceCount; surfaceIndex++)
		{
			XSurface *xsurf = &surfaces[surfaceIndex];

			for (int i = 0; i < 5; i++)
				surfPartBits[i + 4] = xsurf->partBits[i];

			if (boneIndex & 0x1F)
			{
				partBits[0] = (unsigned int)surfPartBits[4 - targBoneIndexHigh] >> targBoneIndexLow;
				partBits[1] = ((unsigned int)surfPartBits[5 - targBoneIndexHigh] >> targBoneIndexLow) | (surfPartBits[4 - targBoneIndexHigh] << invTargBoneIndexLow);
				partBits[2] = ((unsigned int)surfPartBits[6 - targBoneIndexHigh] >> targBoneIndexLow) | (surfPartBits[5 - targBoneIndexHigh] << invTargBoneIndexLow);
				partBits[3] = ((unsigned int)surfPartBits[7 - targBoneIndexHigh] >> targBoneIndexLow) | (surfPartBits[6 - targBoneIndexHigh] << invTargBoneIndexLow);
				partBits[4] = ((unsigned int)surfPartBits[8 - targBoneIndexHigh] >> targBoneIndexLow) | (surfPartBits[7 - targBoneIndexHigh] << invTargBoneIndexLow);
			}
			else
			{
				partBits[0] = surfPartBits[4 - targBoneIndexHigh];
				partBits[1] = surfPartBits[5 - targBoneIndexHigh];
				partBits[2] = surfPartBits[6 - targBoneIndexHigh];
				partBits[3] = surfPartBits[7 - targBoneIndexHigh];
				partBits[4] = surfPartBits[8 - targBoneIndexHigh];
			}

			if (!(partBits[4] & hidePartBits[4] | partBits[3] & hidePartBits[3] | partBits[2] & hidePartBits[2] | partBits[1] & hidePartBits[1] | partBits[0] & hidePartBits[0]))
				streamed &= R_StreamTouchMaterialAndCheck(materials[surfaceIndex], level);
		}

		modelIndex++;
		boneIndex += boneCount;
	}

	return streamed;
}

// /gfx_d3d/r_stream.cpp:1781
void R_StreamTouchMaterial(Material *material)
{
	int imageCount = Material_GetTextureCount(material);

	for (int i = 0; i < imageCount; i++)
		R_StreamTouchImage(Material_GetTexture(material, i));
}

// /gfx_d3d/r_stream.cpp:1794
bool R_StreamTouchMaterialAndCheck(Material *material, int level)
{
	ASSERT_MSG(level >= 0 && level < STREAM_MAX_IMAGE_PARTS, "level doesn't index STREAM_MAX_IMAGE_PARTS");

	bool streamed = true;
	int imageCount = Material_GetTextureCount(material);

	for (int i = 0; i < imageCount; i++)
		streamed &= R_StreamTouchImageAndCheck(Material_GetTexture(material, i), level);

	return streamed;
}

// /gfx_d3d/r_stream.cpp:1814
void R_StreamTouchImage(GfxImage *image)
{
	if (image->streaming == GFX_NOT_STREAMING)
		return;

	int imagePartIndex = DB_GetImageIndex(image);

	for (int part = 0; part < STREAM_MAX_IMAGE_PARTS;)
	{
		// Set the bit to 1/'enable'
		streamFrontendGlob.imageTouchBits[BIT_INDEX_32(imagePartIndex)][streamFrontendGlob.activeImageTouchBits] |= BIT_MASK_32(imagePartIndex);

		part++;
		imagePartIndex++;
	}
}

// /gfx_d3d/r_stream.cpp:1832
bool R_StreamTouchImageAndCheck(GfxImage *image, int level)
{
	ASSERT_MSG(level >= 0 && level < STREAM_MAX_IMAGE_PARTS, "level doesn't index STREAM_MAX_IMAGE_PARTS");

	if (image->streaming == GFX_NOT_STREAMING)
		return true;

	int imagePartIndex = DB_GetImageIndex(image);
	int levelPartIndex = imagePartIndex - ((level > 0) ? 0 : level);

	for (int part = 0; part < STREAM_MAX_IMAGE_PARTS;)
	{
		// Set the bit to 1/'enable'
		streamFrontendGlob.imageTouchBits[BIT_INDEX_32(imagePartIndex)][streamFrontendGlob.activeImageTouchBits] |= BIT_MASK_32(imagePartIndex);

		part++;
		imagePartIndex++;
	}

	return IMAGE_BIT_IS_SET(streamFrontendGlob.imageUseBits, levelPartIndex) != 0;
}

// /gfx_d3d/r_stream.cpp:1860
bool R_StreamImageCheck(GfxImage *image, int level)
{
	ASSERT_MSG(level >= 0 && level < STREAM_MAX_IMAGE_PARTS, "level doesn't index STREAM_MAX_IMAGE_PARTS");

	if (image->streaming == GFX_NOT_STREAMING)
		return true;

	int imageIndex = DB_GetImageIndex(image);
	int levelIndex = (level > 0) ? 0 : level;

	return IMAGE_BIT_IS_SET(streamFrontendGlob.imageUseBits, imageIndex - levelIndex) != 0;
}

// /gfx_d3d/r_stream.cpp:1961
void R_Stream_ResetHintEntities()
{
	s_numStreamHintsActive = 0;
	memset(s_streamHints, 0, sizeof(s_streamHints));
}

// /gfx_d3d/r_stream.cpp:2075
void R_StreamUpdatePreventedMaterials()
{
	memset(streamFrontendGlob.materialPreventBits, 0, sizeof(streamFrontendGlob.materialPreventBits));

	for (int i = 0; i < ARRAYSIZE(s_preventMaterials); i++)
	{
		if (!s_preventMaterials[i])
			continue;

		int materialIndex = DB_GetMaterialIndex(s_preventMaterials[i]);
		IMAGE_BIT_SET(streamFrontendGlob.materialPreventBits, materialIndex);
	}
}

// /gfx_d3d/r_stream.cpp:2101
void R_StreamInit()
{
	R_StreamAlloc_InitTempImages();

	streamFrontendGlob.frame = 0;
	streamFrontendGlob.queryInProgress = 0;
	streamFrontendGlob.queryClient = -1;

	for (int i = 0; i < STREAM_MAX_REQUESTS; i++)
		R_Stream_InvalidateRequest(&s_pendingRequests[i]);

	memset(streamFrontendGlob.imageInSortedListBits, 0, sizeof(streamFrontendGlob.imageInSortedListBits));
	memset(s_preventMaterials, 0, sizeof(s_preventMaterials));

	streamFrontendGlob.sortedImageCount = 0;
	streamFrontendGlob.diskOrderImagesNeedSorting = true;
	streamIsInitialized = true;
}

// /gfx_d3d/r_stream.cpp:2177
void R_StreamShutdown()
{
	R_StreamSetDefaultConfig(true);

	memset(s_preventMaterials, 0, sizeof(s_preventMaterials));
	memset(streamFrontendGlob.imageInSortedListBits, 0, sizeof(streamFrontendGlob.imageInSortedListBits));

	streamFrontendGlob.sortedImageCount = 0;
	streamFrontendGlob.diskOrderImagesNeedSorting = true;
}

// /gfx_d3d/r_stream.cpp:2200
void R_Stream_ReleaseImage(GfxImage *image, bool lock, bool delayDirty)
{
	ASSERT(image);

	if (image->streaming == GFX_NOT_STREAMING)
		return;

	if (lock)
		R_StreamAlloc_Lock();

	int freedSize;
	char *memory = R_StreamAlloc_FreeImage(image, 0, delayDirty, &freedSize);

	if (freedSize > 0)
	{
		ASSERT(memory);
		R_StreamAlloc_Deallocate(memory);
	}

	int imageIndex = DB_GetImageIndex(image);

	for (int part = 0; part < STREAM_MAX_IMAGE_PARTS; part++)
	{
		IMAGE_BIT_UNSET(streamFrontendGlob.imageInitialBits, imageIndex + part);
		IMAGE_BIT_UNSET(streamFrontendGlob.imageForceBits, imageIndex + part);
	}

	streamFrontendGlob.diskOrderImagesNeedSorting = true;

	if (lock)
		R_StreamAlloc_Unlock();
}

// /gfx_d3d/r_stream.cpp:2277
void R_Stream_Sync()
{
	ASSERT(streamIsInitialized);
	ASSERT(!R_StreamIsEnabled());

	if (streamFrontendGlob.queryInProgress)
		R_StreamUpdate_EndQuery();

	ASSERT(!streamFrontendGlob.queryInProgress);

	R_StreamAlloc_Lock();
	for (int i = 0; i < STREAM_MAX_REQUESTS; i++)
	{
		pendingRequest *request = &s_pendingRequests[i];

		switch (request->status)
		{
		case STREAM_STATUS_PRE:
		case STREAM_STATUS_QUEUED:
			IMAGE_BIT_UNSET(streamFrontendGlob.imageLoading, DB_GetImageIndex(request->image));
			R_Stream_InvalidateRequest(request);
			break;

		case STREAM_STATUS_INPROGRESS:
		case STREAM_STATUS_FINISHED:
			IMAGE_BIT_UNSET(streamFrontendGlob.imageLoading, DB_GetImageIndex(request->image));
			Z_VirtualFree(request->buffer, 20);
			R_Stream_InvalidateRequest(request);
			break;
		}
	}
	R_StreamAlloc_Unlock();
}

// /gfx_d3d/r_stream.cpp:2462
void R_StreamSyncThenFlush(bool flushAll)
{
	if (!streamIsInitialized)
		return;

	Sys_EnterCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);
	R_StreamPushDisable();
	R_Stream_Sync();

	if (flushAll)
	{
		R_StreamAlloc_Lock();
		R_StreamAlloc_Flush();
		R_StreamAlloc_Unlock();
	}

	streamFrontendGlob.frame = 0;
	streamFrontendGlob.queryInProgress = 0;
	streamFrontendGlob.calculateTotalBytesWanted = false;
	Sys_EnterCriticalSection(CRITSECT_STREAM_FORCE_LOAD_COMMAND);

	s_numForcedLoadEntities = 0;

	for (int i = 0; i < ARRAYSIZE(s_forcedLoadEntities); i++)
		s_forcedLoadEntities[i] = nullptr;

	Sys_LeaveCriticalSection(CRITSECT_STREAM_FORCE_LOAD_COMMAND);
	R_StreamPopDisable();
	Sys_LeaveCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);
}

// /gfx_d3d/r_stream.cpp:2517
void R_StreamPushSyncDisable()
{
	if (streamIsInitialized)
	{
		Sys_EnterCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);
		R_StreamPushDisable();
		R_Stream_Sync();
		Sys_LeaveCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);
	}
	else
	{
		// Avoid a more expensive sync/critical section when we can
		R_StreamPushDisable();
	}
}

// /gfx_d3d/r_stream.cpp:2532
void R_StreamPopSyncDisable()
{
	ASSERT(!R_StreamIsEnabled());

	if (streamIsInitialized)
	{
		Sys_EnterCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);
		R_Stream_Sync();
		R_StreamPopDisable();
		Sys_LeaveCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);
	}
	else
	{
		// Avoid a more expensive sync/critical section when we can
		R_StreamPopDisable();
	}
}

// /gfx_d3d/r_stream.cpp:2691
void R_CheckHighmipAabbs()
{
	float mins[3];
	mins[0] = -131072.0f;
	mins[1] = -131072.0f;
	mins[2] = -131072.0f;

	float maxs[3];
	maxs[0] = 131072.0f;
	maxs[1] = 131072.0f;
	maxs[2] = 131072.0f;

	ASSERT(false);
	//R_CheckHighmipAabbs_r(rgp_world, 0, mins, maxs);
}

// /gfx_d3d/r_stream.cpp:2816
bool R_StreamUpdate_TryBeginQuery()
{
	// Streaming must be enabled by user
	if (!R_StreamIsEnabled())
		return false;

	// Can't have another query already happening
	if (streamFrontendGlob.queryInProgress)
		return false;

	// Debug dump image list to console
	if (streamFrontendGlob.outputImageList)
	{
		R_ImageList_Output();
		streamFrontendGlob.outputImageList = false;
	}

	memset(streamFrontendGlob.materialImportance, 0, sizeof(streamFrontendGlob.materialImportance));
	memset(streamFrontendGlob.materialImportanceBits, 0, sizeof(streamFrontendGlob.materialImportanceBits));
	memset(streamFrontendGlob.modelDistance, 0, sizeof(streamFrontendGlob.modelDistance));
	memset(streamFrontendGlob.modelDistanceBits, 0, sizeof(streamFrontendGlob.modelDistanceBits));
	memset(streamFrontendGlob.imageImportance, 0, sizeof(streamFrontendGlob.imageImportance));
	memset(streamFrontendGlob.imageImportanceBits, 0, sizeof(streamFrontendGlob.imageImportanceBits));
	memset(streamFrontendGlob.dynamicModelDistance, 0, sizeof(streamFrontendGlob.dynamicModelDistance));
	memset(streamFrontendGlob.dynamicModelDistanceBits, 0, sizeof(streamFrontendGlob.dynamicModelDistanceBits));
	return true;
}

// /gfx_d3d/r_stream.cpp:2895
void R_StreamUpdate_Idle()
{
	if (!DB_FinishedLoadingAssets())
		return;

	if (R_StreamUpdate(nullptr))
		R_StreamUpdate_EndQuery();
}

// /gfx_d3d/r_stream.cpp:2909
void R_StreamUpdate_CompletePreload(void(__cdecl *pumpfunc)())
{
	R_BeginRemoteScreenUpdate();

	while (R_IsRemoteScreenUpdateActive())
	{
		if (!R_StreamIsEnabled())
			break;

		if (streamFrontendGlob.preloadCancelled)
			break;

		R_StreamAlloc_Lock();
		float progress = R_Stream_GetProgress();
		R_StreamAlloc_Unlock();

		if (progress >= 1.0f || streamFrontendGlob.initialLoadAllocFailures > 0)
			break;

		Sleep(10);
	}

	R_EndRemoteScreenUpdate(pumpfunc);
}

// /gfx_d3d/r_stream.cpp:3170
bool R_StreamUpdate(const float *viewPos)
{
	bool updateCalled = false;

	if (r_streamClear->current.enabled || r_stream->modified)
	{
		R_StreamSyncThenFlush(true);
		Dvar_ClearModified(r_streamSize);
		Dvar_ClearModified(r_stream);
		Dvar_SetBool(r_streamClear, false);
	}

	if (r_streamSize->modified)
	{
		R_StreamSyncThenFlush(false);
		Dvar_ClearModified(r_streamSize);
	}

	if (r_streamLowDetail->modified)
	{
		if (r_streamLowDetail->current.enabled)
			R_Stream_ForceLoadLowDetail();
		else
			memset(streamFrontendGlob.imageForceBits, 0, sizeof(streamFrontendGlob.imageForceBits));

		Dvar_ClearModified(r_streamLowDetail);
		streamFrontendGlob.diskOrderImagesNeedSorting = true;
	}

	if (r_reflectionProbeGenerate->current.enabled)
		return false;

	if (r_stream->current.integer > 0)
		updateCalled = R_StreamUpdate_FindImageAndOptimize(viewPos);

	return updateCalled;
}

// /gfx_d3d/r_stream.cpp:3344
void R_Stream_AddImagePartImportance(int imagePartIndex, float importance)
{
	ASSERT_MSG(imagePartIndex >= 0 && imagePartIndex < STREAM_MAX_IMAGES, "imagePartIndex doesn't index TOTAL_IMAGE_PARTS");

	streamFrontendGlob.imageImportance[imagePartIndex] = max(importance, streamFrontendGlob.imageImportance[imagePartIndex]);

	if (!IMAGE_BIT_IS_SET(streamFrontendGlob.imageImportanceBits, imagePartIndex))
	{
		IMAGE_BIT_SET(streamFrontendGlob.imageImportanceBits, imagePartIndex);

		if (!IMAGE_BIT_IS_SET(streamFrontendGlob.imageInSortedListBits, imagePartIndex))
		{
			IMAGE_BIT_SET(streamFrontendGlob.imageInSortedListBits, imagePartIndex);
			streamFrontendGlob.sortedImages[streamFrontendGlob.sortedImageCount++] = (__int16)imagePartIndex;
		}
	}

	ASSERT(streamFrontendGlob.sortedImageCount < ARRAYSIZE(streamFrontendGlob.sortedImages));
}

// /gfx_d3d/r_stream.cpp:3361
void R_StreamTouchImagesFromMaterial(Material *remoteMaterial, float importance)
{
	int textureCount = remoteMaterial->textureCount;

	if (!textureCount)
		return;

	for (int textureIter = 0; textureIter != textureCount; textureIter++)
	{
		MaterialTextureDef *texDef = &remoteMaterial->textureTable[textureIter];
		ASSERT(texDef);

		if (texDef->semantic == TS_WATER_MAP)
			continue;

		GfxImage *image = texDef->u.image;
		ASSERT(image);

		if (image->streaming)
		{
			int imageIndex = DB_GetImageIndex(image);
			R_Stream_AddImagePartImportance(imageIndex, importance);
		}
	}
}

// /gfx_d3d/r_stream.cpp:3454
float FastPointDistSqFromBounds(const float4& mins, const float4& maxs)
{
	float4 rvec;
	rvec.v[0] = min(maxs.v[0], max(s_viewPos.v[0], mins.v[0]));
	rvec.v[1] = min(maxs.v[1], max(s_viewPos.v[1], mins.v[1]));
	rvec.v[2] = min(maxs.v[2], max(s_viewPos.v[2], mins.v[2]));

	return
		(((rvec.v[0] - s_viewPos.v[0]) * (rvec.v[0] - s_viewPos.v[0])) +
		((rvec.v[1] - s_viewPos.v[1]) * (rvec.v[1] - s_viewPos.v[1]))) +
		((rvec.v[2] - s_viewPos.v[2]) * (rvec.v[2] - s_viewPos.v[2]));
}

// /gfx_d3d/r_stream.cpp:3461
void MultiplePointDistSqFromBounds(distance_data *distances, const float *v, const float *mip0mins, const float *mip0maxs, float himipRadiusSq, float distanceScale)
{
	STATIC_ASSERT_SIZE(float4, sizeof(float) * 4);

	// These variables are expected to be 16-byte aligned
	ASSERT(((size_t)(&s_viewPos.v[0]) & 15) == 0);
	ASSERT(((size_t)((void*)mip0mins) & 15) == 0);
	ASSERT(((size_t)((void*)mip0maxs) & 15) == 0);

	float4 mins;
	memcpy(mins.v, mip0mins, sizeof(float) * 4);

	float4 maxs;
	memcpy(maxs.v, mip0maxs, sizeof(float) * 4);

	distances->distanceForHimip = FastPointDistSqFromBounds(mins, maxs) * distanceScale;
	distances->importance = himipRadiusSq / (distances->distanceForHimip + 100.0f);
}

// /gfx_d3d/r_stream.cpp:3491
void R_StreamUpdateForXModel(XModel *remoteModel, float distSq)
{
	const float distNotSq = sqrt(distSq);

	Material **materialHandles = remoteModel->materialHandles;
	XModelHighMipBounds *highMipBounds = remoteModel->streamInfo.highMipBounds;

	for (int surf = 0; surf < remoteModel->numsurfs; surf++)
	{
		if (!materialHandles[surf]->textureCount)
			continue;

		XModelHighMipBounds *bounds = &highMipBounds[surf];
		
		if (highMipBounds[surf].himipRadiusSq != 0.0f)
		{
			float distanceForHimip = distNotSq - Vec3Length(bounds->center);

			float importance = bounds->himipRadiusSq / (max(distanceForHimip * distanceForHimip, 0.0f) + 100.0f);
			int materialIndex = DB_GetMaterialIndex(materialHandles[surf]);

			ASSERT(materialIndex >= 0);
			ASSERT(materialIndex < MAX_MATERIAL_POOL_SIZE);

			streamFrontendGlob.materialImportance[materialIndex] = max(importance, streamFrontendGlob.materialImportance[materialIndex]);
		}
	}
}

// /gfx_d3d/r_stream.cpp:3539
void R_StreamUpdateForXModelTouched(XModel *model)
{
	if (!model)
		return;

	for (int lod = 0; lod < MAX_LODS; lod++)
	{
		XSurface *surfaces = nullptr;
		Material * const *material = nullptr;

		int surfCount = XModelGetSurfaces(model, &surfaces, lod);
		material = XModelGetSkins(model, lod);

		for (int surfIter = 0; surfIter != surfCount;)
		{
			R_StreamTouchMaterial(*material);
			surfIter++;
			material++;
		}
	}
}

// /gfx_d3d/r_stream.cpp:3561
void R_StreamUpdateTouchedModels()
{
	for (int modelIndex = 0; modelIndex < STREAM_MAX_MODELS; modelIndex++)
	{
		if (!IMAGE_BIT_IS_SET(streamFrontendGlob.modelTouchBits, modelIndex))
			continue;

		XModel *model = DB_GetXModelAtIndex(modelIndex);
		ASSERT(model);

		R_StreamUpdateForXModelTouched(model);
	}
}

// /gfx_d3d/r_stream.cpp:3585
void R_StreamUpdateForBModel(const float *viewPos, unsigned int frame, unsigned int surfId, GfxBrushModel *bmodel, const float *origin, float maxDistSq, Material *altMaterial, bool isVisible, float *distanceScale)
{
	__debugbreak();
}

// /gfx_d3d/r_stream.cpp:3652
void R_StreamUpdate_AddXModelDistance(XModel *model, const float *viewPos, const float *origin, const float scale, bool visible, float *distanceScale)
{
	int modelIndex = DB_GetXModelIndex(model);
	float distSq = (Vec3DistanceSq(viewPos, origin) / scale) * distanceScale[visible];

	if (IMAGE_BIT_IS_SET(streamFrontendGlob.modelDistanceBits, modelIndex))
	{
		streamFrontendGlob.modelDistance[modelIndex] = min(distSq, streamFrontendGlob.modelDistance[modelIndex]);
	}
	else
	{
		streamFrontendGlob.modelDistance[modelIndex] = distSq;
		IMAGE_BIT_SET(streamFrontendGlob.modelDistanceBits, modelIndex);
	}
}

// /gfx_d3d/r_stream.cpp:3670
void R_StreamUpdate_AddDynamicXModelDistance(XModel *model, const float *viewPos, const float *origin, const float scale, bool visible, float *distanceScale)
{
	int modelIndex = DB_GetXModelIndex(model);
	float distSq = (Vec3DistanceSq(viewPos, origin) / scale) * distanceScale[visible];

	if (IMAGE_BIT_IS_SET(streamFrontendGlob.dynamicModelDistanceBits, modelIndex))
	{
		streamFrontendGlob.dynamicModelDistance[modelIndex] = min(distSq, streamFrontendGlob.dynamicModelDistance[modelIndex]);
	}
	else
	{
		streamFrontendGlob.dynamicModelDistance[modelIndex] = distSq;
		IMAGE_BIT_SET(streamFrontendGlob.dynamicModelDistanceBits, modelIndex);
	}
}

// /gfx_d3d/r_stream.cpp:3690
void R_StreamUpdateDynamicModels(const float *viewPos, float maxDistSq, unsigned int frame, float *distanceScale)
{
	memcpy((void *)0x03E5B664, &s_viewPos, 16);

	static DWORD dwCall = 0x006FA4F0;

	__asm
	{
		mov edi, viewPos
		push distanceScale
		call [dwCall]
		add esp, 0x4
	}
}

// /gfx_d3d/r_stream.cpp:3802
void R_StreamUpdateStaticModel(int staticModelIndex, const float *viewPos, float maxDistSq, float *distanceScale)
{
	R_StreamUpdate_AddXModelDistance(
		rgp_world->dpvs.smodelDrawInsts[staticModelIndex].model,
		viewPos,
		rgp_world->dpvs.smodelDrawInsts[staticModelIndex].placement.origin,
		rgp_world->dpvs.smodelDrawInsts[staticModelIndex].placement.scale,
		((0x80000000 >> (staticModelIndex & 0x1F)) & rgp_world->dpvs.smodelVisDataCameraSaved[staticModelIndex >> 5]) != 0,
		distanceScale);
}

// /gfx_d3d/r_stream.cpp:3824
void R_StreamUpdateWorldSurface(int surfId, const float *viewPos, float maxDistSq, float *distanceScale)
{
	//ASSERT(surfId >= 0 && surfId < rgp_world->surfaceCount);

	GfxSurface *surface = &rgp_world->dpvs.surfaces[surfId];
	bool isVisible = rgp_world->dpvs.surfaceVisDataCameraSaved[surfId] != 0;

	if (!*(DWORD *)0x396EE10 || isVisible)
	{
		distance_data distSq;

		MultiplePointDistSqFromBounds(
			&distSq,
			viewPos,
			surface->tris.mins,
			surface->tris.maxs,
			surface->tris.himipRadiusSq,
			distanceScale[isVisible]);

		int materialIndex = DB_GetMaterialIndex(surface->material);
		streamFrontendGlob.materialImportance[materialIndex] = max(distSq.importance, streamFrontendGlob.materialImportance[materialIndex]);
	}
}

// /gfx_d3d/r_stream.cpp:3888
void R_StreamUpdate_CombineImportance()
{
	for (int modelIndex = 0; modelIndex < STREAM_MAX_MODELS; modelIndex++)
	{
		unsigned int mask = BIT_MASK_32(modelIndex);
		unsigned int staticBit = mask & streamFrontendGlob.modelDistanceBits[BIT_INDEX_32(modelIndex)];
		unsigned int dynamicBit = mask & streamFrontendGlob.dynamicModelDistanceBits[BIT_INDEX_32(modelIndex)];

		XModel *model = DB_GetXModelAtIndex(modelIndex);

		if (dynamicBit & staticBit)
		{
			ASSERT(model);
			R_StreamUpdateForXModel(model, min(streamFrontendGlob.dynamicModelDistance[modelIndex], streamFrontendGlob.modelDistance[modelIndex]));
		}
		else if (staticBit)
		{
			ASSERT(model);
			R_StreamUpdateForXModel(model, streamFrontendGlob.modelDistance[modelIndex]);
		}
		else if (dynamicBit)
		{
			ASSERT(model);
			R_StreamUpdateForXModel(model, streamFrontendGlob.dynamicModelDistance[modelIndex]);
		}
	}

	for (int materialIndex = 0; materialIndex < STREAM_MAX_MATERIALS; materialIndex++)
	{
		if (streamFrontendGlob.materialImportance[materialIndex] != 0.0f
			&& !IMAGE_BIT_IS_SET(streamFrontendGlob.materialPreventBits, materialIndex))
		{
			Material *material = DB_GetMaterialAtIndex(materialIndex);
			ASSERT(material);

			R_StreamTouchImagesFromMaterial(material, streamFrontendGlob.materialImportance[materialIndex]);
		}
	}
}

// /gfx_d3d/r_stream.cpp:3953
void R_StreamUpdateAabbNode_r_0_(int aabbTreeNode, const float *viewPos, float maxDistSq, float *distanceScale)
{
	ASSERT(aabbTreeNode >= 0 && aabbTreeNode < rgp_world->streamInfo.aabbTreeCount);

	GfxStreamingAabbTree *tree = &rgp_world->streamInfo.aabbTrees[aabbTreeNode];
	float distSq = PointDistSqFromBounds(viewPos, tree->mins, tree->maxs);

	if (distSq > maxDistSq)
		return;

	if (tree->childCount)
	{
		int childBegin = tree->firstChild;
		int childEnd = childBegin + tree->childCount;

		for (int childIter = childBegin; childIter != childEnd; childIter++)
			R_StreamUpdateAabbNode_r_0_(childIter, viewPos, maxDistSq, distanceScale);
	}
	else
	{
		int *leafRefBegin = &rgp_world->streamInfo.leafRefs[tree->firstItem];
		int *leafRefEnd = leafRefBegin + tree->itemCount;

		for (int *leafRefIter = leafRefBegin; leafRefIter != leafRefEnd; leafRefIter++)
		{
			int ref = *leafRefIter;

			if (ref < 0)
				R_StreamUpdateStaticModel(~ref, viewPos, maxDistSq, distanceScale);
			else
				R_StreamUpdateWorldSurface(ref, viewPos, maxDistSq, distanceScale);
		}
	}
}

// /gfx_d3d/r_stream.cpp:4020
void R_StreamUpdateStatic(const float *viewPos, float maxDistSq, float *distanceScale)
{
	ASSERT(viewPos);

	s_viewPos.v[0] = viewPos[0];
	s_viewPos.v[1] = viewPos[1];
	s_viewPos.v[2] = viewPos[2];

	if (rgp_world->streamInfo.aabbTreeCount > 0)
		R_StreamUpdateAabbNode_r_0_(0, viewPos, maxDistSq, distanceScale);
}

// /gfx_d3d/r_stream.cpp:4084
void importance_swap_func(void **a, void **b)
{
	void *temp = *a;
	*a = *b;
	*b = temp;
}

// /gfx_d3d/r_stream.cpp:4092
bool importance_compare_func(void *a, void *b)
{
	const int indexA = (int)a;
	const int indexB = (int)b;

	return *(int *)&streamFrontendGlob.imageImportance[indexA] > *(int *)&streamFrontendGlob.imageImportance[indexB];
}

struct importance_and_offset_pred
{
	importance_and_offset_pred(bool DiskOrder)
	{
	}

	inline bool operator() (const int& a, const int& b)
	{
		return importance_compare_func((void *)a, (void *)b);
	}
};

// /gfx_d3d/r_stream.cpp:4099
void importance_merge_sort(void **list, const int list_count)
{
	static void *aux_buffer[STREAM_MAX_IMAGES];

	if (list_count >= 3)
	{
		importance_merge_sort(list, list_count / 2);
		importance_merge_sort(&list[list_count / 2], list_count - list_count / 2);

		void **b = &list[list_count / 2];
		void **a = &list[list_count / 2 - 1];

		if (importance_compare_func(list[list_count / 2], *a))
		{
			void **t = aux_buffer;
			aux_buffer[0] = *a;

			for (a = &list[list_count / 2 - 2]; a >= list && importance_compare_func(*b, *a); --a)
			{
				++t;
				*t = *a;
			}

			++a;
			*a = *b;
			++b;

			while (t >= aux_buffer && b < &list[list_count])
			{
				++a;
				if (importance_compare_func(*t, *b))
				{
					*a = *t;
					--t;
				}
				else
				{
					*a = *b;
					++b;
				}
			}
			while (t >= aux_buffer)
			{
				++a;
				*a = *t;
				--t;
			}
		}
	}
	else if (list_count == 2 && importance_compare_func(list[1], list[0]))
	{
		importance_swap_func(list, list + 1);
	}
}

// /gfx_d3d/r_stream.cpp:4127
void R_StreamUpdate_EndQuerySort(bool diskOrder)
{
	for (int index = 0; index < streamFrontendGlob.sortedImageCount;)
	{
		int imagePartIndex = streamFrontendGlob.sortedImages[index];

		ASSERT(IMAGE_BIT_IS_SET(streamFrontendGlob.imageInSortedListBits, imagePartIndex));

		if (IMAGE_BIT_IS_SET(streamFrontendGlob.imageImportanceBits, imagePartIndex))
		{
			index++;
		}
		else
		{
			streamFrontendGlob.sortedImages[index] = streamFrontendGlob.sortedImages[--streamFrontendGlob.sortedImageCount];
			IMAGE_BIT_UNSET(streamFrontendGlob.imageInSortedListBits, imagePartIndex);
		}
	}

	if (diskOrder)
	{
		if (streamFrontendGlob.diskOrderImagesNeedSorting || streamFrontendGlob.forceDiskOrder)
		{
			std::sort(
				streamFrontendGlob.sortedImages,
				streamFrontendGlob.sortedImages + streamFrontendGlob.sortedImageCount,
				importance_and_offset_pred(diskOrder));
			streamFrontendGlob.diskOrderImagesNeedSorting = false;
		}
	}
	else
	{
		importance_merge_sort((void **)streamFrontendGlob.sortedImages, streamFrontendGlob.sortedImageCount);
	}
}

// /gfx_d3d/r_stream.cpp:4254
void R_StreamUpdateForcedModels(unsigned int frame)
{
	int surfCountPrevLods = 0;
	Sys_EnterCriticalSection(CRITSECT_STREAM_FORCE_LOAD_COMMAND);

	for (int i = 0; i < s_numForcedLoadEntities; i++)
	{
		int modelCount = DObjGetNumModels(s_forcedLoadEntities[i]);
		float distSq = streamFrontendGlob.forcedImageImportance;

		for (int modelIter = 0; modelIter != modelCount; modelIter++)
		{
			XModel *model = DObjGetModel(s_forcedLoadEntities[i], modelIter);

			for (int lod = 0; lod < MAX_LODS; lod++)
			{
				XSurface *surfaces = nullptr;
				Material * const *material = nullptr;

				int surfCount = XModelGetSurfaces(model, &surfaces, lod);
				material = XModelGetSkins(model, lod);

				for (int surfIter = 0; surfIter != surfCount;)
				{
					if ((*material)->textureCount)
						R_StreamTouchImagesFromMaterial(*material, distSq);

					surfIter++;
					material++;
				}

				surfCountPrevLods += surfCount;
			}
		}
	}

	Sys_LeaveCriticalSection(CRITSECT_STREAM_FORCE_LOAD_COMMAND);
}

// /gfx_d3d/r_stream.cpp:4300
void R_StreamUpdate_EndQuery_Internal()
{
	if (Sys_IsRenderThread())
		R_StreamUpdate_ProcessFileCallbacks();

	for (int sortedIndex = 0; sortedIndex != streamFrontendGlob.sortedImageCount;)
	{
		// Find the next applicable stream load request
		pendingRequest *request = nullptr;

		for (int i = 0; i < STREAM_MAX_REQUESTS; i++)
		{
			if (request || s_pendingRequests[i].status != STREAM_STATUS_INVALID || i >= STREAM_MAX_REQUESTS)
			{
				if (s_pendingRequests[i].status == STREAM_STATUS_PRE)
					R_StreamRequestImageRead(&s_pendingRequests[i]);
			}
			else
			{
				request = &s_pendingRequests[i];
			}
		}

		if (!request)
			return;

		if (sortedIndex < streamFrontendGlob.sortedImageCount)
		{
			int imagePartIndex = 0;
			GfxImage *image = nullptr;

			while (true)
			{
				imagePartIndex = streamFrontendGlob.sortedImages[sortedIndex];

				const unsigned int imageIndex = BIT_INDEX_32(imagePartIndex);
				const unsigned int imageMask = BIT_MASK_32(imagePartIndex);

				if (!(streamFrontendGlob.imageUseBits[imageIndex] & imageMask) && !(streamFrontendGlob.imageLoading[imageIndex] & imageMask))
				{
					image = DB_GetImageAtIndex(imagePartIndex);

					if (image->streaming)
					{
						if (image->streaming != GFX_TEMP_STREAMING && image->skippedMipLevels)
							break;
					}
				}

				if (++sortedIndex >= streamFrontendGlob.sortedImageCount)
					goto LABEL_23;
			}

			stream_status status = R_StreamRequestImageAllocation(request, image, true, streamFrontendGlob.imageImportance[imagePartIndex]);

			if (status == STREAM_STATUS_INPROGRESS)
			{
				streamFrontendGlob.initialLoadAllocFailures = 0;

				if (!R_StreamRequestImageRead(request))
					return;

			LABEL_23:
				continue;
			}

			// bool GfxGlobals::isMultiplayer;
			if (!*(bool *)0x396A4B1 || status != STREAM_STATUS_READFAILED && status != STREAM_STATUS_CANCELLED)
				streamFrontendGlob.initialLoadAllocFailures++;

			return;
		}
	}
}

// /gfx_d3d/r_stream.cpp:4516
void R_StreamUpdate_EndQuery()
{
	ASSERT(streamFrontendGlob.queryInProgress == 1);

	Sys_WaitWorkerCmdInternal(&r_stream_sortWorkerCmd);

	streamFrontendGlob.queryInProgress = 0;

	R_StreamAlloc_Lock();
	R_StreamUpdate_EndQuery_Internal();
	R_StreamAlloc_Unlock();
}

// /gfx_d3d/r_stream.cpp:4531
bool R_StreamUpdate_FindImageAndOptimize(const float *viewPos)
{
	ASSERT(streamIsInitialized);

	// Don't do anything if user froze image streaming globally
	if (r_streamFreezeState->current.enabled)
		return false;

	if (r_streamCheckAabb->current.enabled)
		R_CheckHighmipAabbs();

	Sys_EnterCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);

	if (streamFrontendGlob.queryInProgress)
		R_StreamUpdate_EndQuery();

	if (!R_StreamUpdate_TryBeginQuery())
	{
		Sys_LeaveCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);
		return false;
	}

	ASSERT(streamFrontendGlob.queryClient == -1);

	float maxDistSq = r_streamMaxDist->current.value * r_streamMaxDist->current.value;
	streamFrontendGlob.queryInProgress = 1;
	streamFrontendGlob.queryClient = 0;

	// If there was no client active screen pos (before renderer is fully loaded), take the easy way out and sort
	if (!rgp_world || !viewPos)
	{
		streamFrontendGlob.frame++;
		streamFrontendGlob.diskOrder = true;

		R_StreamUpdate_AddForcedImages(
			streamFrontendGlob.forcedImageImportance,
			streamFrontendGlob.touchedImageImportance);

		R_StreamUpdate_AddInitialImages(streamFrontendGlob.initialImageImportance);

		StreamSortCmd sortCmd;
		sortCmd.frontend = &streamFrontendGlob;
		sortCmd.diskOrder = true;
		Sys_AddWorkerCmdInternal(&r_stream_sortWorkerCmd, &sortCmd, nullptr);
		streamFrontendGlob.queryClient = -1;

		Sys_LeaveCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);
		return true;
	}

	// Otherwise use the world position/distance to optimize sorting
	streamFrontendGlob.diskOrderImagesNeedSorting = true;
	streamFrontendGlob.frame++;
	streamFrontendGlob.diskOrder = streamFrontendGlob.forceDiskOrder;

	R_StreamUpdatePreventedMaterials();
	R_StreamUpdateForcedModels(streamFrontendGlob.frame);
	R_StreamUpdateTouchedModels();

	memset(streamFrontendGlob.modelTouchBits, 0, sizeof(streamFrontendGlob.modelTouchBits));

	R_StreamUpdate_AddForcedImages(
		streamFrontendGlob.forcedImageImportance,
		streamFrontendGlob.touchedImageImportance);

	ASSERT(viewPos);

	StreamUpdateCmd updateCmd;
	updateCmd.frontend = &streamFrontendGlob;
	updateCmd.viewPos[0] = viewPos[0];
	updateCmd.viewPos[1] = viewPos[1];
	updateCmd.viewPos[2] = viewPos[2];
	updateCmd.maxDistSq = maxDistSq;
	updateCmd.distanceScale[1] = 1.0f;
	updateCmd.distanceScale[0] = r_streamHiddenPush->current.value;
	Sys_AddWorkerCmdInternal(&r_stream_updateWorkerCmd, &updateCmd, nullptr);

	for (int hint = 0; hint < ARRAYSIZE(s_streamHints); hint++)
	{
		if (s_streamHints[hint].importance <= 0.0f)
			continue;

		updateCmd.distanceScale[1] = 1.0f / s_streamHints[hint].importance;
		updateCmd.distanceScale[0] = updateCmd.distanceScale[1];
		updateCmd.viewPos[0] = s_streamHints[hint].origin[0];
		updateCmd.viewPos[1] = s_streamHints[hint].origin[1];
		updateCmd.viewPos[2] = s_streamHints[hint].origin[2];
		Sys_AddWorkerCmdInternal(&r_stream_updateWorkerCmd, &updateCmd, nullptr);
	}

	Sys_LeaveCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);
	return true;
}

// /gfx_d3d/r_stream.cpp:4644
void R_StreamUpdatePerClient(const float *viewPos)
{
	ASSERT(streamIsInitialized);

	if (r_streamFreezeState->current.enabled || !rgp_world || !viewPos)
		return;

	Sys_EnterCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);

	// Exit the function if there's no query active at all
	if (!streamFrontendGlob.queryInProgress || streamFrontendGlob.queryClient == -1)
	{
		Sys_LeaveCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);
		return;
	}

	float maxDistSq = r_streamMaxDist->current.value * r_streamMaxDist->current.value;

	if (streamFrontendGlob.queryClient > 0)
	{
		ASSERT(viewPos);

		StreamUpdateCmd updateCmd;
		updateCmd.frontend = &streamFrontendGlob;
		updateCmd.viewPos[0] = viewPos[0];
		updateCmd.viewPos[1] = viewPos[1];
		updateCmd.viewPos[2] = viewPos[2];
		updateCmd.maxDistSq = maxDistSq;
		updateCmd.distanceScale[1] = 1.0f;
		updateCmd.distanceScale[0] = r_streamHiddenPush->current.value;
		Sys_AddWorkerCmdInternal(&r_stream_updateWorkerCmd, &updateCmd, nullptr);
	}

	float distanceScale[2];
	distanceScale[1] = 1.0f;
	distanceScale[0] = r_streamHiddenPush->current.value;
	R_StreamUpdateDynamicModels(viewPos, maxDistSq, streamFrontendGlob.frame, distanceScale);
	streamFrontendGlob.queryClient++;

	Sys_LeaveCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);
}

// /gfx_d3d/r_stream.cpp:4705
void R_StreamUpdate_End()
{
	ASSERT(streamFrontendGlob.queryClient > 0);

	Sys_EnterCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);

	StreamCombineCmd combineCmd;
	combineCmd.frontend = &streamFrontendGlob;
	Sys_AddWorkerCmdInternal(&r_stream_combineWorkerCmd, &combineCmd, nullptr);

	StreamSortCmd sortCmd;
	sortCmd.frontend = &streamFrontendGlob;
	sortCmd.diskOrder = streamFrontendGlob.diskOrder;
	Sys_AddWorkerCmdInternal(&r_stream_sortWorkerCmd, &sortCmd, nullptr);

	streamFrontendGlob.queryClient = -1;
	Sys_LeaveCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);
}

// /gfx_d3d/r_stream.cpp:4729
void R_Stream_UpdateStaticModelsCmd(void *data)
{
	StreamUpdateCmd *cmd = (StreamUpdateCmd *)data;

	s_viewPos.v[0] = cmd->viewPos[0];
	s_viewPos.v[1] = cmd->viewPos[1];
	s_viewPos.v[2] = cmd->viewPos[2];

	for (unsigned int instId = 0; instId < g_worldDpvs->smodelCount; instId++)
		R_StreamUpdateStaticModel(instId, cmd->viewPos, cmd->maxDistSq, cmd->distanceScale);
}

// /gfx_d3d/r_stream.cpp:4770
void R_Stream_UpdateStaticSurfacesCmd(void *data)
{
	StreamUpdateCmd *cmd = (StreamUpdateCmd *)data;

	s_viewPos.v[0] = cmd->viewPos[0];
	s_viewPos.v[1] = cmd->viewPos[1];
	s_viewPos.v[2] = cmd->viewPos[2];

	for (unsigned int surfId = 0; surfId < g_worldDpvs->staticSurfaceCount; surfId++)
		R_StreamUpdateWorldSurface(surfId, cmd->viewPos, cmd->maxDistSq, cmd->distanceScale);
}

// /gfx_d3d/r_stream.cpp:4808
void R_Stream_SortCmd(void *data)
{
	StreamSortCmd *cmd = (StreamSortCmd *)data;

	R_StreamUpdate_EndQuerySort(cmd->diskOrder);
}

// /gfx_d3d/r_stream.cpp:4807
void R_Stream_CombineCmd(void *data)
{
	R_StreamUpdate_CombineImportance();
}

// /gfx_d3d/r_stream.cpp:4905
int r_stream_update_staticmodelsCallback(jqBatch *batch)
{
	// This command doesn't allow multiple module instances (s_viewPos)
	if (*(DWORD *)(batch->Module + 20) > 1)
		return 1;

	void *data = jqLockData(batch);
	R_Stream_UpdateStaticModelsCmd(data);
	jqUnlockData(batch, data);
	return 0;
}

// /gfx_d3d/r_stream.cpp:4917
int r_stream_update_staticsurfacesCallback(jqBatch *batch)
{
	// This command doesn't allow multiple module instances (s_viewPos)
	if (*(DWORD *)(batch->Module + 20) > 1)
		return 1;

	void *data = jqLockData(batch);
	R_Stream_UpdateStaticSurfacesCmd(data);
	jqUnlockData(batch, data);
	return 0;
}

// /gfx_d3d/r_stream.cpp:4929
int r_stream_sortCallback(jqBatch *batch)
{
	if (jqPoll((jqBatchGroup *)0xBA5EB0) || jqPoll((jqBatchGroup *)0xBA5E30))
		return 1;

	void *data = jqLockData(batch);
	R_Stream_SortCmd(data);
	jqUnlockData(batch, data);
	return 0;
}

// /gfx_d3d/r_stream.cpp:4941
int r_stream_combineCallback(jqBatch *batch)
{
	if (jqPoll((jqBatchGroup *)0xBA5E30) || jqPoll((jqBatchGroup *)0xBA5E50) || jqPoll((jqBatchGroup *)0xBA5E70))
		return 1;

	void *data = jqLockData(batch);
	R_Stream_CombineCmd(data);
	jqUnlockData(batch, data);
	return 0;
}

// /gfx_d3d/r_stream.cpp:4953
int r_stream_updateCallback(jqBatch *batch)
{
	// This command doesn't allow multiple module instances (s_viewPos)
	if (*(DWORD *)(batch->Module + 20) > 1)
		return 1;

	void *data = jqLockData(batch);
	StreamUpdateCmd *cmd = (StreamUpdateCmd *)data;

	s_viewPos.v[0] = cmd->viewPos[0];
	s_viewPos.v[1] = cmd->viewPos[1];
	s_viewPos.v[2] = cmd->viewPos[2];
	R_StreamUpdateStatic(cmd->viewPos, cmd->maxDistSq, cmd->distanceScale);

	jqUnlockData(batch, data);
	return 0;
}

GfxImage *CL_CompositeSetupImage()
{
	return R_StreamAlloc_SetupTempImage(D3DFMT_A8R8G8B8, false, 128, 128, 5);
}

void CL_CompositeSetupImageCallback(void *param)
{
	*(GfxImage **)param = CL_CompositeSetupImage();
}

void __declspec(naked) hk_R_StreamTouchImageAndCheck()
{
	__asm
	{
		push ecx
		push eax
		call R_StreamTouchImageAndCheck
		add esp, 0x8
		retn
	}
}

void __declspec(naked) hk_R_StreamUpdatePerClient()
{
	__asm
	{
		push eax
		call R_StreamUpdatePerClient
		add esp, 0x4
		retn
	}
}

void Patch_R_Stream()
{
#define DO_NOT_USE(x) /*PatchMemory((x), (PBYTE)"\xCC", 1);*/
	DO_NOT_USE(0x00472980);
	DO_NOT_USE(0x004F3E30);

	DO_NOT_USE(0x0052C280);
	DO_NOT_USE(0x005FB280);

	DO_NOT_USE(0x004561D0);
	DO_NOT_USE(0x005A1DC0);

	// CL_CompositeSetupImageCallback for R_StreamAlloc_SetupTempImage
	Detours::X86::DetourFunction((PBYTE)0x00635AD0, (PBYTE)&CL_CompositeSetupImageCallback);

	DO_NOT_USE(0x006F8BF0);
	DO_NOT_USE(0x006F8C50); Detours::X86::DetourFunction((PBYTE)0x006F8C50, (PBYTE)&R_StreamSetInitData);
	DO_NOT_USE(0x006F8C60);
	DO_NOT_USE(0x006F8D00);
	DO_NOT_USE(0x006F8D70); Detours::X86::DetourFunction((PBYTE)0x006F8D70, (PBYTE)&R_StreamUpdate_ReadTextures);
	DO_NOT_USE(0x006F8EB0);
	DO_NOT_USE(0x006F8FE0); Detours::X86::DetourFunction((PBYTE)0x006F8FE0, (PBYTE)&R_StreamUpdate_ProcessFileCallbacks);
	DO_NOT_USE(0x006F91A0); Detours::X86::DetourFunction((PBYTE)0x006F91A0, (PBYTE)&R_StreamUpdate_SetupInitialImageList);
	DO_NOT_USE(0x006F9250);
	DO_NOT_USE(0x006F94E0); Detours::X86::DetourFunction((PBYTE)0x006F94E0, (PBYTE)&R_StreamTouchDObjAndCheck);
	DO_NOT_USE(0x006F9760); Detours::X86::DetourFunction((PBYTE)0x006F9760, (PBYTE)&R_StreamTouchMaterial);
	DO_NOT_USE(0x006F97D0); Detours::X86::DetourFunction((PBYTE)0x006F97D0, (PBYTE)&R_StreamTouchMaterialAndCheck);
	DO_NOT_USE(0x006F9880); Detours::X86::DetourFunction((PBYTE)0x006F9880, (PBYTE)&hk_R_StreamTouchImageAndCheck); // USERCALL
	DO_NOT_USE(0x006F98F0);
	DO_NOT_USE(0x006F9950); Detours::X86::DetourFunction((PBYTE)0x006F9950, (PBYTE)&R_StreamInit);
	DO_NOT_USE(0x006F99D0); Detours::X86::DetourFunction((PBYTE)0x006F99D0, (PBYTE)&R_StreamShutdown);
	DO_NOT_USE(0x006F9A70); Detours::X86::DetourFunction((PBYTE)0x006F9A70, (PBYTE)&R_Stream_ReleaseImage); // BROKEN
	DO_NOT_USE(0x006F9B00);
	DO_NOT_USE(0x006F9C10); Detours::X86::DetourFunction((PBYTE)0x006F9C10, (PBYTE)&R_StreamSyncThenFlush);
	DO_NOT_USE(0x006F9CC0); Detours::X86::DetourFunction((PBYTE)0x006F9CC0, (PBYTE)&R_StreamPushSyncDisable);
	DO_NOT_USE(0x006F9D00); Detours::X86::DetourFunction((PBYTE)0x006F9D00, (PBYTE)&R_StreamPopSyncDisable);
	DO_NOT_USE(0x006F9D40);
	DO_NOT_USE(0x006F9DF0); Detours::X86::DetourFunction((PBYTE)0x006F9DF0, (PBYTE)&R_StreamUpdate_Idle);
	DO_NOT_USE(0x006F9E10); Detours::X86::DetourFunction((PBYTE)0x006F9E10, (PBYTE)&R_StreamUpdate_CompletePreload);
	DO_NOT_USE(0x006F9E80); Detours::X86::DetourFunction((PBYTE)0x006F9E80, (PBYTE)&R_StreamUpdate);
	DO_NOT_USE(0x006F9F60);
	DO_NOT_USE(0x006F9FE0);
	DO_NOT_USE(0x006FA030);
	DO_NOT_USE(0x006FA130);
	DO_NOT_USE(0x006FA260);
	DO_NOT_USE(0x006FA2B0);
	DO_NOT_USE(0x006FA2F0);
	DO_NOT_USE(0x006FA440);
	DO_NOT_USE(0x006FA4F0);
	DO_NOT_USE(0x006FA9F0);
	DO_NOT_USE(0x006FAA40);
	DO_NOT_USE(0x006FAB10);
	DO_NOT_USE(0x006FAC20);
	DO_NOT_USE(0x006FAD40);
	DO_NOT_USE(0x006FAE00);
	DO_NOT_USE(0x006FAF10);
	DO_NOT_USE(0x006FB040);
	DO_NOT_USE(0x006FB080);
	DO_NOT_USE(0x006FB2C0); Detours::X86::DetourFunction((PBYTE)0x006FB2C0, (PBYTE)&hk_R_StreamUpdatePerClient); // USERCALL
	DO_NOT_USE(0x006FB3D0); Detours::X86::DetourFunction((PBYTE)0x006FB3D0, (PBYTE)&R_StreamUpdate_End);
	DO_NOT_USE(0x006FB430);
	DO_NOT_USE(0x006FB570);
	DO_NOT_USE(0x006FB5C0); Detours::X86::DetourFunction((PBYTE)0x006FB5C0, (PBYTE)&r_stream_update_staticmodelsCallback);
	DO_NOT_USE(0x006FB5F0); Detours::X86::DetourFunction((PBYTE)0x006FB5F0, (PBYTE)&r_stream_update_staticsurfacesCallback);
	DO_NOT_USE(0x006FB620); Detours::X86::DetourFunction((PBYTE)0x006FB620, (PBYTE)&r_stream_sortCallback);
	DO_NOT_USE(0x006FB670); Detours::X86::DetourFunction((PBYTE)0x006FB670, (PBYTE)&r_stream_combineCallback);
	DO_NOT_USE(0x006FB6D0); Detours::X86::DetourFunction((PBYTE)0x006FB6D0, (PBYTE)&r_stream_updateCallback);
	DO_NOT_USE(0x006FB770);
	DO_NOT_USE(0x006FB8D0);
	DO_NOT_USE(0x006FBEC0);
	DO_NOT_USE(0x006FBF00);
	DO_NOT_USE(0x006FBFA0); Detours::X86::DetourFunction((PBYTE)0x006FBFA0, (PBYTE)&R_StreamAlloc_ReleaseTempImage);
	DO_NOT_USE(0x006FBFF0);
	DO_NOT_USE(0x006FC0C0);

	// Patch inlined R_StreamAlloc_Lock/R_StreamAlloc_Unlock in RB_RenderThread
	PatchMemory_WithNOP(0x006EBE67, 5); Detours::X86::DetourFunction((PBYTE)0x006EBE67, (PBYTE)&R_StreamAlloc_Lock, Detours::X86Option::USE_CALL);
	PatchMemory_WithNOP(0x006EBE71, 12); Detours::X86::DetourFunction((PBYTE)0x006EBE71, (PBYTE)&R_StreamAlloc_Unlock, Detours::X86Option::USE_CALL);

	PatchMemory_WithNOP(0x006EBF45, 5); Detours::X86::DetourFunction((PBYTE)0x006EBF45, (PBYTE)&R_StreamAlloc_Lock, Detours::X86Option::USE_CALL);
	PatchMemory_WithNOP(0x006EBF4F, 12); Detours::X86::DetourFunction((PBYTE)0x006EBF4F, (PBYTE)&R_StreamAlloc_Unlock, Detours::X86Option::USE_CALL);

	// Patch streamFrontendGlob.frame reference in SCR_UpdateFrame
	void *ptr = &streamFrontendGlob.frame;
	PatchMemory(0x007A17D1 + 0x2, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x007A188C + 0x2, (PBYTE)&ptr, sizeof(DWORD));

	// R_StreamUpdateDynamicModels
	ptr = &streamFrontendGlob.dynamicModelDistanceBits;
	PatchMemory(0x006FA5E7 + 0x3, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x006FA6E7 + 0x3, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x006FA84E + 0x3, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x006FA8EC + 0x3, (PBYTE)&ptr, sizeof(DWORD));

	ptr = &streamFrontendGlob.dynamicModelDistance;
	PatchMemory(0x006FA5F5 + 0x5, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x006FA611 + 0x5, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x006FA61C + 0x5, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x006FA6F5 + 0x5, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x006FA711 + 0x5, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x006FA71C + 0x5, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x006FA869 + 0x5, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x006FA885 + 0x5, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x006FA890 + 0x5, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x006FA907 + 0x5, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x006FA923 + 0x5, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x006FA92E + 0x5, (PBYTE)&ptr, sizeof(DWORD));

	// R_Stream_AddImagePartImportance for R_StreamUpdateForBModel for R_StreamUpdateDynamicModels
	ptr = &streamFrontendGlob.imageImportance;
	PatchMemory(0x006F9F60 + 0x5, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x006F9F91 + 0x5, (PBYTE)&ptr, sizeof(DWORD));

	ptr = &streamFrontendGlob.imageImportanceBits;
	PatchMemory(0x006F9F9A + 0x2, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x006F9FA6 + 0x2, (PBYTE)&ptr, sizeof(DWORD));

	ptr = &streamFrontendGlob.imageInSortedListBits;
	PatchMemory(0x006F9FAC + 0x2, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x006F9FB8 + 0x2, (PBYTE)&ptr, sizeof(DWORD));

	ptr = &streamFrontendGlob.sortedImages;
	PatchMemory(0x006F9FC7 + 0x3, (PBYTE)&ptr, sizeof(DWORD));

	ptr = &streamFrontendGlob.sortedImageCount;
	PatchMemory(0x006F9FBE + 0x2, (PBYTE)&ptr, sizeof(DWORD));
	PatchMemory(0x006F9FCE + 0x2, (PBYTE)&ptr, sizeof(DWORD));
}