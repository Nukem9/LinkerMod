#include "stdafx.h"

#define IMAGE_BIT_IS_SET(var, bits) ((var)[BIT_INDEX_32(bits)] & BIT_MASK_32(bits))
#define IMAGE_BIT_SET(var, bits)	((var)[BIT_INDEX_32(bits)] |= BIT_MASK_32(bits));
#define IMAGE_BIT_UNSET(var, bits)	((var)[BIT_INDEX_32(bits)] &= ~BIT_MASK_32(bits));

int& rgp = *(int *)0x0396A24C;

StreamFrontendGlob streamFrontendGlob;
pendingRequest s_pendingRequests[10];

DObj *s_forcedLoadEntities[4];
int s_numForcedLoadEntities;

streamerHintInfo s_streamHints[4];
int s_numStreamHintsActive;

Material *s_preventMaterials[32];

bool streamIsInitialized;
volatile long disableCount;

float4& s_viewPos = *(float4 *)0x03E5B664;

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

		for (int idx = 0; idx < 132; idx++)
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

	for (int i = 0; i < ARRAYSIZE(s_pendingRequests); i++)
	{
		int idx = (lastIdx + i) % ARRAYSIZE(s_pendingRequests);

		if (s_pendingRequests[idx].status != STREAM_STATUS_QUEUED)
			continue;

		if (!request)
		{
			request = &s_pendingRequests[idx];
			request->status = STREAM_STATUS_INPROGRESS;
			lastIdx = (idx + 1) % ARRAYSIZE(s_pendingRequests);
		}

		numRequests++;
	}
	R_StreamAlloc_Unlock();

	if (request)
	{
		if (Image_LoadToBuffer(request->image, request->highMip, &request->buffer, &request->bufferSize))
			request->status = STREAM_STATUS_FINISHED;
		else
			request->status = STREAM_STATUS_CANCELLED;

		Sleep(1);
	}

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
		pendingRequest *unloadRequest = nullptr;

		for (int i = 0; i < ARRAYSIZE(s_pendingRequests); i++)
		{
			if (s_pendingRequests[i].status == STREAM_STATUS_INVALID)
			{
				unloadRequest = &s_pendingRequests[i];
				break;
			}
		}

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

// /gfx_d3d/r_stream.cpp:1169
bool R_StreamUpdate_ProcessFileCallbacks()
{
	bool workDone = false;

	for (int i = 0; i < ARRAYSIZE(s_pendingRequests); i++)
	{
		pendingRequest *request = &s_pendingRequests[i];

		if (request->status == STREAM_STATUS_INVALID ||
			request->status == STREAM_STATUS_PRE ||
			request->status == STREAM_STATUS_QUEUED ||
			request->status == STREAM_STATUS_INPROGRESS)
			continue;

		GfxImage *image = request->image;

		ASSERT(image != nullptr);

		int imageIndex = DB_GetImageIndex(image);

		ASSERT(IMAGE_BIT_IS_SET(streamFrontendGlob.imageLoading, imageIndex));

		if (request->status != STREAM_STATUS_CANCELLED && request->status != STREAM_STATUS_READFAILED && request->status != STREAM_STATUS_EOF)
		{
			workDone = true;

			ASSERT(request->status == STREAM_STATUS_FINISHED);

			if (Sys_IsRenderThread())
			{
				GfxImageFileHeader *fileHeader = (GfxImageFileHeader *)request->buffer;
				char *imageData = request->buffer + sizeof(GfxImageFileHeader);

				Image_Release(image);

				ASSERT(!image->texture.basemap);

				Image_LoadFromData(image, fileHeader, imageData, 2);
				Z_VirtualFree(request->buffer, 20);
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
		else
		{
			IMAGE_BIT_UNSET(streamFrontendGlob.imageLoading, imageIndex);
			Z_VirtualFree(request->buffer, 20);
			R_Stream_InvalidateRequest(request);
		}
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
	for (int imagePartIndex = 0; imagePartIndex < TOTAL_IMAGE_PARTS; imagePartIndex++)
	{
		if (IMAGE_BIT_IS_SET(streamFrontendGlob.imageInitialBits, imagePartIndex))
			R_Stream_AddImagePartImportance(imagePartIndex, importance);
	}
}

// /gfx_d3d/r_stream.cpp:1474
void R_StreamUpdate_AddForcedImages(float forceImportance, float touchImportance)
{
	unsigned int v2; // eax@3
	int v3; // eax@5
	signed int v4; // ecx@10
	int v5; // eax@13
	int v6; // eax@25
	signed int mask; // [sp+1Ch] [bp-1Ch]@7
	unsigned int useBits; // [sp+20h] [bp-18h]@3
	unsigned int forceBits; // [sp+24h] [bp-14h]@3
	unsigned int touchBits; // [sp+28h] [bp-10h]@3
	int part; // [sp+2Ch] [bp-Ch]@12
	signed int index; // [sp+34h] [bp-4h]@1

	streamFrontendGlob.activeImageTouchBits ^= 1;

	for (int index = 0; index < 132; index++)
	{
		touchBits = streamFrontendGlob.imageTouchBits[66][index] | streamFrontendGlob.imageTouchBits[0][index];
		forceBits = streamFrontendGlob.imageForceBits[index] | touchBits;
		useBits = streamFrontendGlob.imageUseBits[index] & ~forceBits;
		v2 = streamFrontendGlob.imageUseBits[index] & ~forceBits;

		if (!_BitScanReverse((DWORD *)&v2, forceBits))
			v2 = 0x3F;
		v3 = v2 ^ 0x1F;

		for (int bitIndex = 31 - v3; bitIndex >= 0; bitIndex = 31 - v3)
		{
			mask = 1 << (bitIndex & 0x1F);

			ASSERT(forceBits & BIT_MASK_32(bitIndex));

			forceBits &= ~mask;

			v4 = (bitIndex + 32 * index) & 0x80000000;
			if (v4 < 0)
				v4 = 0;
			part = v4 + 1;

			if (mask & touchBits)
				R_Stream_AddImagePartImportance(bitIndex + 32 * index, touchImportance / (float)part);
			else
				R_Stream_AddImagePartImportance(bitIndex + 32 * index, forceImportance / (float)part);

			if (!_BitScanReverse((DWORD *)&v5, forceBits))
				v5 = 0x3F;
			v3 = v5 ^ 0x1F;
		}

		if (!_BitScanReverse((DWORD *)&v3, useBits))
			v3 = 0x3F;

		for (int bitIndex = 31 - (v3 ^ 0x1F); bitIndex >= 0; bitIndex = 31 - (v6 ^ 0x1F))
		{
			ASSERT(useBits & BIT_MASK_32(bitIndex));

			useBits &= ~BIT_MASK_32(bitIndex);

			R_Stream_AddImagePartImportance(bitIndex + 32 * index, 0.0f);

			if (!_BitScanReverse((DWORD *)&v6, useBits))
				v6 = 0x3F;
		}
	}

	memset(streamFrontendGlob.imageTouchBits[66 * (streamFrontendGlob.activeImageTouchBits ^ 1)], 0, 0x210u);
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
			for (int partIndex = 0; partIndex < part && partIndex < TOTAL_IMAGE_PARTS; partIndex++)
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
		for (int partIndex = 0; partIndex < TOTAL_IMAGE_PARTS; partIndex++)
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
	ASSERT_MSG(level >= 0 && level < MAX_IMAGE_STREAMED_PARTS, "level doesn't index MAX_IMAGE_STREAMED_PARTS");

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

	for (int part = 0; part < MAX_IMAGE_STREAMED_PARTS;)
	{
		IMAGE_BIT_SET(streamFrontendGlob.imageTouchBits[66 * streamFrontendGlob.activeImageTouchBits], imagePartIndex);

		part++;
		imagePartIndex++;
	}
}

// /gfx_d3d/r_stream.cpp:1832
bool R_StreamTouchImageAndCheck(GfxImage *image, int level)
{
	ASSERT_MSG(level >= 0 && level < MAX_IMAGE_STREAMED_PARTS, "level doesn't index MAX_IMAGE_STREAMED_PARTS");

	if (image->streaming == GFX_NOT_STREAMING)
		return true;

	int imagePartIndex = DB_GetImageIndex(image);
	int levelPartIndex = imagePartIndex - ((level > 0) ? 0 : level);

	for (int part = 0; part < MAX_IMAGE_STREAMED_PARTS;)
	{
		IMAGE_BIT_SET(streamFrontendGlob.imageTouchBits[66 * streamFrontendGlob.activeImageTouchBits], imagePartIndex);

		part++;
		imagePartIndex++;
	}

	return IMAGE_BIT_IS_SET(streamFrontendGlob.imageUseBits, levelPartIndex) != 0;
}

// /gfx_d3d/r_stream.cpp:1860
bool R_StreamImageCheck(GfxImage *image, int level)
{
	ASSERT_MSG(level >= 0 && level < MAX_IMAGE_STREAMED_PARTS, "level doesn't index MAX_IMAGE_STREAMED_PARTS");

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

	for (int i = 0; i < ARRAYSIZE(s_pendingRequests); i++)
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

	for (int part = 0; part < MAX_IMAGE_STREAMED_PARTS; part++)
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
	for (int i = 0; i < ARRAYSIZE(s_pendingRequests); i++)
	{
		pendingRequest *request = &s_pendingRequests[i];

		if (request->status != STREAM_STATUS_PRE && request->status != STREAM_STATUS_QUEUED)
		{
			if (request->status == STREAM_STATUS_FINISHED)
			{
				IMAGE_BIT_UNSET(streamFrontendGlob.imageLoading, DB_GetImageIndex(request->image));

				Z_VirtualFree(request->buffer, 20);
				R_Stream_InvalidateRequest(request);
			}
			else if (request->status == STREAM_STATUS_INPROGRESS)
			{
				IMAGE_BIT_UNSET(streamFrontendGlob.imageLoading, DB_GetImageIndex(request->image));

				Z_VirtualFree(request->buffer, 20);
				R_Stream_InvalidateRequest(request);
			}
		}
		else
		{
			int imageIndex = DB_GetImageIndex(request->image);
			IMAGE_BIT_UNSET(streamFrontendGlob.imageLoading, DB_GetImageIndex(request->image));
			R_Stream_InvalidateRequest(request);
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
	//R_CheckHighmipAabbs_r(rgp, 0, mins, maxs);
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
	ASSERT_MSG(imagePartIndex >= 0 && imagePartIndex < TOTAL_IMAGE_PARTS, "imagePartIndex doesn't index TOTAL_IMAGE_PARTS");

	streamFrontendGlob.imageImportance[imagePartIndex] = max(importance, streamFrontendGlob.imageImportance[imagePartIndex]);

	if (!IMAGE_BIT_IS_SET(streamFrontendGlob.imageImportanceBits, imagePartIndex))
	{
		IMAGE_BIT_SET(streamFrontendGlob.imageImportanceBits, imagePartIndex);

		if (!IMAGE_BIT_IS_SET(streamFrontendGlob.imageInSortedListBits, imagePartIndex))
		{
			IMAGE_BIT_SET(streamFrontendGlob.imageInSortedListBits, imagePartIndex);
			streamFrontendGlob.sortedImages[streamFrontendGlob.sortedImageCount++] = (signed __int16)imagePartIndex;
		}
	}
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
float FastPointDistSqFromBounds(float4 mins, float4 maxs)
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

	ASSERT(((unsigned int)(&s_viewPos.v[0]) & 15) == 0);
	ASSERT(((unsigned int)((void*)mip0mins) & 15) == 0);
	ASSERT(((unsigned int)((void*)mip0maxs) & 15) == 0);

	float4 maxs;
	memcpy(maxs.v, mip0maxs, sizeof(float) * 4);

	float4 mins;
	memcpy(mins.v, mip0mins, sizeof(float) * 4);

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
		
		// CHECK LAHF
		if (highMipBounds[surf].himipRadiusSq != 0.0f)
		{
			float distanceForHimip = distNotSq - Vec3Length(bounds->center);

			float importance = bounds->himipRadiusSq / (min(distanceForHimip * distanceForHimip, 0.0f) + 100.0f);
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
	for (int modelIndex = 0; modelIndex < MAX_XMODEL_POOL_SIZE; modelIndex++)
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
	// Doesn't reference streamFrontendGlob. Not re-implemented.
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
	// Doesn't reference streamFrontendGlob. Not re-implemented.
	__debugbreak();
}

// /gfx_d3d/r_stream.cpp:3802
void R_StreamUpdateStaticModel(int staticModelIndex, const float *viewPos, float maxDistSq, float *distanceScale)
{
	// Doesn't reference streamFrontendGlob. Not re-implemented.
	__debugbreak();
}

// /gfx_d3d/r_stream.cpp:3824
void R_StreamUpdateWorldSurface(int surfId, const float *viewPos, float maxDistSq, float *distanceScale)
{
	//ASSERT(surfId >= 0 && surfId < rgp.world->surfaceCount);

	GfxSurface *surface = (GfxSurface *)(*(DWORD *)(rgp + 0x39C) + 0x50 * surfId);
	bool isVisible = *(BYTE *)(surfId + *(DWORD *)(rgp + 0x38C));

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
	for (int modelIndex = 0; modelIndex < MAX_XMODEL_POOL_SIZE; modelIndex++)
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

	for (int materialIndex = 0; materialIndex < MAX_MATERIAL_POOL_SIZE; materialIndex++)
	{
		// CHECK LAHF
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
	// Doesn't reference streamFrontendGlob. Not re-implemented.
	__debugbreak();
}

// /gfx_d3d/r_stream.cpp:4020
void R_StreamUpdateStatic(const float *viewPos, float maxDistSq, float *distanceScale)
{
	ASSERT(viewPos);

	// Doesn't reference streamFrontendGlob. Not re-implemented.
	__debugbreak();
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
	return streamFrontendGlob.imageImportance[(int)a] > streamFrontendGlob.imageImportance[(int)b];
}

// /gfx_d3d/r_stream.cpp:4099
void importance_merge_sort(void **list, const int list_count)
{
	static void *aux_buffer[8000];

	void **t; // [sp+0h] [bp-14h]@7
	void **b; // [sp+8h] [bp-Ch]@6
	void **ba; // [sp+8h] [bp-Ch]@11
	void **a; // [sp+10h] [bp-4h]@6
	void **aa; // [sp+10h] [bp-4h]@7
	void **ab; // [sp+10h] [bp-4h]@11

	if (list_count >= 3)
	{
		importance_merge_sort(list, list_count / 2);
		importance_merge_sort(&list[list_count / 2], list_count - list_count / 2);

		b = &list[list_count / 2];
		a = &list[list_count / 2 - 1];

		if (importance_compare_func(list[list_count / 2], *a))
		{
			t = aux_buffer;
			aux_buffer[0] = *a;
			for (aa = &list[list_count / 2 - 2]; aa >= list && importance_compare_func(*b, *aa); --aa)
			{
				++t;
				*t = *aa;
			}
			ab = aa + 1;
			*ab = *b;
			ba = b + 1;
			while (t >= aux_buffer && ba < &list[list_count])
			{
				++ab;
				if (importance_compare_func(*t, *ba))
				{
					*ab = *t;
					--t;
				}
				else
				{
					*ab = *ba;
					++ba;
				}
			}
			while (t >= aux_buffer)
			{
				++ab;
				*ab = *t;
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
			((void(__cdecl *)(int *, int *, int, bool))0x006FB8D0)(streamFrontendGlob.sortedImages, &streamFrontendGlob.sortedImages[streamFrontendGlob.sortedImageCount], 4 * streamFrontendGlob.sortedImageCount >> 2, diskOrder);
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
	bool v4 = false;
	int sortedIndex = 0;

	if (Sys_IsRenderThread())
		R_StreamUpdate_ProcessFileCallbacks();

	if (streamFrontendGlob.sortedImageCount)
	{
		do
		{
			// Find the next applicate stream load request
			pendingRequest *request = nullptr;

			for (int i = 0; i < ARRAYSIZE(s_pendingRequests); i++)
			{
				if (request || s_pendingRequests[i].status || i >= ARRAYSIZE(s_pendingRequests))
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

			v4 = sortedIndex == streamFrontendGlob.sortedImageCount;
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
					v4 = sortedIndex == streamFrontendGlob.sortedImageCount;
					continue;
				}

				// bool GfxGlobals::isMultiplayer;
				if (!*(bool *)0x396A4B1 || status != STREAM_STATUS_READFAILED && status != STREAM_STATUS_CANCELLED)
					streamFrontendGlob.initialLoadAllocFailures++;

				return;
			}
		} while (!v4);
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
	if (!rgp || !viewPos)
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
		Sys_AddWorkerCmdInternal(&r_stream_sortWorkerCmd, &sortCmd, 0);
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

	if (!r_streamFreezeState->current.enabled || !rgp || !viewPos)
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
	// Doesn't reference streamFrontendGlob. Not re-implemented.
	__debugbreak();
}

// /gfx_d3d/r_stream.cpp:4770
void R_Stream_UpdateStaticSurfacesCmd(void *data)
{
	// Doesn't reference streamFrontendGlob. Not re-implemented.
	__debugbreak();
}

// /gfx_d3d/r_stream.cpp:4808
void R_Stream_SortCmd(void *data)
{
	R_StreamUpdate_EndQuerySort(*((bool *)data + 4));
}

// /gfx_d3d/r_stream.cpp:4807
void R_Stream_CombineCmd(void *data)
{
	R_StreamUpdate_CombineImportance();
}