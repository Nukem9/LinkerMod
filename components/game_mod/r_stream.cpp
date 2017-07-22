#include "stdafx.h"

#if 0
volatile int disableCount;

enum stream_status
{
	STREAM_STATUS_INVALID = 0x0,
	STREAM_STATUS_PRE = 0x1,
	STREAM_STATUS_QUEUED = 0x2,
	STREAM_STATUS_INPROGRESS = 0x3,
	STREAM_STATUS_CANCELLED = 0x4,
	STREAM_STATUS_READFAILED = 0x5,
	STREAM_STATUS_EOF = 0x6,
	STREAM_STATUS_FINISHED = 0x7,
	STREAM_STATUS_USER1 = 0x8,
	STREAM_STATUS_COUNT = 0x9,
};

struct pendingRequest
{
	int id[1];
	GfxImage *image;
	int imagePart;
	char *buffer;
	int bufferSize;
	int startTime;
	float importance;
	stream_status status;
	stream_status partStatus[1];
	int numParts;
	int bytesToCopy;
	bool highMip;
};

bool R_StreamIsEnabled()
{
	return disableCount == 0;
}

void R_StreamPushDisable()
{
	InterlockedIncrement((LONG *)&disableCount);
}

void R_StreamPopDisable()
{
	int value = InterlockedDecrement((LONG *)&disableCount);

	ASSERT_MSG(value >= 0, "disableCount >= 0");
}

void R_StreamSetInitData(int buffer_size)
{
	streamFrontendGlob.mainBufferSize = buffer_size;
}

void R_StreamSetDefaultConfig(bool clear)
{
	streamFrontendGlob.forceDiskOrder = false;
	streamFrontendGlob.touchedImageImportance = FLT_MAX;

	//*(_QWORD *)&streamFrontendGlob.initialImageImportance = __PAIR__(
	//	LODWORD(FLOAT_1_7014117e38),
	//	LODWORD(FLOAT_8_5070587e37));

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

void R_StreamSetUIConfig(bool clear)
{
	R_StreamSetDefaultConfig(clear);

	streamFrontendGlob.forceDiskOrder = true;
	//streamFrontendGlob.touchedImageImportance = FLOAT_1_7014117e38;
	//streamFrontendGlob.forcedImageImportance = FLOAT_0_0000099999997;
	//streamFrontendGlob.initialImageImportance = FLOAT_8_5070587e37;
}

float R_Stream_GetProgress()
{
	double result; // st7@2
	unsigned int bits; // ST34_4@9
	signed int complete; // [sp+28h] [bp-10h]@7
	signed int total; // [sp+2Ch] [bp-Ch]@7
	signed int idx; // [sp+34h] [bp-4h]@7

	if (!com_waitForStreamer->current.integer)
		return 1.0f;

		if (streamFrontendGlob.initialLoadAllocFailures <= 0)
		{
			if (streamFrontendGlob.imageInitialBitsSet)
			{
				total = 0;
				complete = 0;
				for (idx = 0; idx < 132; ++idx)
				{
					bits = streamFrontendGlob.imageInitialBits[idx] | streamFrontendGlob.imageForceBits[idx];
					total += CountBitsEnabled(bits);
					complete += CountBitsEnabled(streamFrontendGlob.imageUseBits[idx] & bits);
				}
				if (total)
					result = (double)complete / (double)total + 0.001;
				else
					result = 1.0;
			}
			else
			{
				result = 0.0;
			}
		}
		else
		{
			result = 1.0;
		}

	return result;
}

void R_StreamUpdate_ReadTextures()
{
	pendingRequest *request = nullptr;
	int numRequests = 0;

	R_StreamAlloc_Lock();
	for (int i = 0; i < 10; i++)
	{
		int idx = (unk_ADD7110 + i) % 10;

		if (s_pendingRequests[idx].status == 2)
		{
			if (!request)
			{
				request = &s_pendingRequests[idx];
				s_pendingRequests[idx].status = 3;
				unk_ADD7110 = (idx + 1) % 10;
			}

			++numRequests;
		}
	}
	R_StreamAlloc_Unlock();

	if (request)
	{
		if (Image_LoadToBuffer(request->image, request->highMip, &request->buffer, &request->bufferSize))
			request->status = STREAM_STATUS_FINISHED;
		else
			request->status = STREAM_STATUS_CANCELLED;

		NET_Sleep(1);
	}

	if (numRequests > 1)
		Sys_WakeStream();
}

bool R_StreamUpdate_ProcessFileCallbacks()
{
	bool workDone = false;

	for (int i = 0; i < 10; i++)
	{
		pendingRequest *request = &s_pendingRequests[i];

		if (s_pendingRequests[i].status && request->status != 1 && request->status != 2 && request->status != 3)
		{
			GfxImage *image = request->image;

			ASSERT(image != nullptr);

			int imageIndex = DB_GetImageIndex(image);

			ASSERT(streamFrontendGlob.imageLoading[BIT_INDEX_32(imageIndex)] & BIT_MASK_32(imageIndex));

			if (request->status != STREAM_STATUS_CANCELLED && request->status != STREAM_STATUS_READFAILED && request->status != STREAM_STATUS_EOF)
			{
				workDone = true;

				ASSERT(request->status == STREAM_STATUS_FINISHED);

				if (Sys_IsRenderThread())
				{
					GfxImageFileHeader *fileHeader = (GfxImageFileHeader *)request->buffer;
					char *imageData = request->buffer + 48;

					Image_Release(image);

					ASSERT(!image->texture.basemap);

					Image_LoadFromData(image, fileHeader, imageData, 2u);
					Z_VirtualFree(request->buffer, 20);
				}
				else
				{
					RB_Resource_ReloadTexture(image, request->buffer);
				}

				if (request->highMip)
				{
					ASSERT(!(streamFrontendGlob.imageUseBits[BIT_INDEX_32(imageIndex)] & BIT_MASK_32(imageIndex)));
					streamFrontendGlob.imageUseBits[imageIndex >> 5] |= 1 << (imageIndex & 0x1F);
				}
				else
				{
					ASSERT(streamFrontendGlob.imageUseBits[BIT_INDEX_32(imageIndex)] & BIT_MASK_32(imageIndex));
					streamFrontendGlob.imageUseBits[imageIndex >> 5] &= ~(1 << (imageIndex & 0x1F));
				}

				streamFrontendGlob.imageLoading[imageIndex >> 5] &= ~(1 << (imageIndex & 0x1F));
				R_Stream_InvalidateRequest(request);
			}
			else
			{
				streamFrontendGlob.imageLoading[imageIndex >> 5] &= ~(1 << (imageIndex & 0x1F));
				Z_VirtualFree(request->buffer, 20);
				R_Stream_InvalidateRequest(request);
			}
		}
	}

	return workDone;
}

void R_Stream_InvalidateRequest(pendingRequest *request)
{
	memset(request, 0, sizeof(pendingRequest));
	request->status = STREAM_STATUS_INVALID;
}

void R_StreamUpdate_SetupInitialImageList()
{
	R_StreamSetDefaultConfig(true);

	if (r_streamLowDetail && r_streamLowDetail->current.enabled)
		BG_EvalVehicleName();
	else
		memset(streamFrontendGlob.imageForceBits, 0, sizeof(streamFrontendGlob.imageForceBits));

	streamFrontendGlob.diskOrderImagesNeedSorting = true;
	streamFrontendGlob.imageInitialBitsSet = true;
	streamFrontendGlob.initialLoadAllocFailures = 0;
	streamFrontendGlob.preloadCancelled = true;
}

void R_Stream_ForceLoadMaterial(Material *material, int part)
{
	if (!material)
		return;

	int textureCount = material->textureCount;

	for (int textureIter = 0; textureIter != textureCount; textureIter++)
	{
		MaterialTextureDef *texDef = &material->textureTable[textureIter];

		ASSERT(texDef);

		if (texDef->semantic == 11)
			continue;

		ASSERT(texDef->u.image);
		R_Stream_ForceLoadImage(texDef->u.image, part);
	}
}

void R_Stream_ForceLoadImage(GfxImage *image, int part)
{
	if (!image || !image->streaming)
		return;

	if (part)
	{
		if (part >= 0)
		{
			for (int partIndex = 0; partIndex < part && partIndex < 1; partIndex++)
			{
				int v4 = DB_GetImageIndex(image);
				streamFrontendGlob.imageForceBits[(signed int)(partIndex + v4) >> 5] |= 1 << ((partIndex + v4) & 0x1F);
			}
		}
		else
		{
			int partIndex = (part + 1 < 0) ? 0 : part + 1;

			for (; partIndex >= 0; partIndex--)
			{
				int v3 = DB_GetImageIndex(image);
				streamFrontendGlob.imageForceBits[(signed int)(partIndex + v3) >> 5] |= 1 << ((partIndex + v3) & 0x1F);
			}
		}
	}
	else
	{
		for (int partIndex = 0; partIndex < 1; partIndex++)
		{
			int v2 = DB_GetImageIndex(image);
			streamFrontendGlob.imageForceBits[(signed int)(partIndex + v2) >> 5] &= ~(1 << ((partIndex + v2) & 0x1F));
		}
	}

	streamFrontendGlob.diskOrderImagesNeedSorting = 1;
}

void R_Stream_ForceLoadModel(XModel *model, int part)
{
	if (!model)
		return;

	for (int lod = 0; lod < 4; lod++)
	{
		XSurface *surfaces = nullptr;
		Material **material = nullptr;

		int surfCount = XModelGetSurfaces(model, &surfaces, lod);
		material = (Material **)XModelGetSkins(model, lod);

		for (int surfIter = 0; surfIter != surfCount;)
		{
			R_Stream_ForceLoadMaterial(*material, part);
			surfIter++;
			material++;
		}
	}
}

bool __cdecl R_StreamTouchDObjAndCheck(DObj *obj, int level)
{
	signed int i; // [sp+4h] [bp-8Ch]@30
	unsigned int boneIndex; // [sp+Ch] [bp-84h]@10
	XModel *model; // [sp+10h] [bp-80h]@15
	char streamed; // [sp+17h] [bp-79h]@13
	int boneCount; // [sp+18h] [bp-78h]@18
	unsigned int surfaceIndex; // [sp+1Ch] [bp-74h]@27
	unsigned int surfaceCount; // [sp+20h] [bp-70h]@18
	char targBoneIndexLow; // [sp+24h] [bp-6Ch]@24
	char invTargBoneIndexLow; // [sp+28h] [bp-68h]@24
	Material *const *materials; // [sp+2Ch] [bp-64h]@18
	char *surfaces; // [sp+30h] [bp-60h]@18
	int hidePartBits[5]; // [sp+34h] [bp-5Ch]@24
	unsigned int targBoneIndexHigh; // [sp+48h] [bp-48h]@24
	int surfPartBits[9]; // [sp+4Ch] [bp-44h]@13
	char *xsurf; // [sp+70h] [bp-20h]@30
	int partBits[5]; // [sp+74h] [bp-1Ch]@34
	unsigned int modelCount; // [sp+88h] [bp-8h]@10
	unsigned int modelIndex; // [sp+8Ch] [bp-4h]@13

	ASSERT(obj);
	ASSERT(Sys_IsMainThread());
	ASSERT(IsFastFileLoad());

	boneIndex = 0;
	modelCount = DObjGetNumModels(obj);

	ASSERT(modelCount <= DOBJ_MAX_SUBMODELS);

	surfPartBits[0] = 0;
	surfPartBits[1] = 0;
	surfPartBits[2] = 0;
	surfPartBits[3] = 0;
	surfPartBits[4] = 0;
	surfPartBits[5] = 0;
	surfPartBits[6] = 0;
	surfPartBits[7] = 0;
	surfPartBits[8] = 0;
	streamed = 1;
	modelIndex = 0;

	while (modelIndex < modelCount)
	{
		model = DObjGetModel(obj, modelIndex);

		ASSERT(model);

		materials = XModelGetSkins(model, 0);
		boneCount = XModelNumBones((cpose_t *)model);
		surfaceCount = XModelGetSurfaces(model, (XSurface **)&surfaces, 0);

		ASSERT(surfaces);
		ASSERT(surfaceCount);

		targBoneIndexHigh = boneIndex >> 5;
		targBoneIndexLow = boneIndex & 0x1F;

		invTargBoneIndexLow = 32 - (boneIndex & 0x1F);
		DObjGetHidePartBits(obj, (unsigned int *)hidePartBits);

		ASSERT(surfaceCount <= DOBJ_MAX_SURFS);

		for (surfaceIndex = 0; surfaceIndex < surfaceCount; surfaceIndex++)
		{
			xsurf = &surfaces[68 * surfaceIndex];
			for (i = 0; i < 5; ++i)
				surfPartBits[i + 4] = *(_DWORD *)&xsurf[4 * i + 48];
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
		++modelIndex;
		boneIndex += boneCount;
	}
	return streamed;
}

void R_StreamTouchMaterial(Material *material)
{
	int imageCount = Material_GetTextureCount(material);

	for (int i = 0; i < imageCount; i++)
		R_StreamTouchImage(Material_GetTexture(material, i));
}

bool R_StreamTouchMaterialAndCheck(Material *material, int level)
{
	ASSERT(level >= 0 && level < 1, "level doesn't index MAX_IMAGE_STREAMED_PARTS");

	bool streamed = true;
	int imageCount = Material_GetTextureCount(material);

	for (i = 0; i < imageCount; i++)
		streamed &= R_StreamTouchImageAndCheck(Material_GetTexture(material, i), level);

	return streamed;
}

void R_StreamTouchImage(GfxImage *image)
{
	if (!image->streaming)
		return;

	int imagePartIndex = DB_GetImageIndex(image);

	for (int part = 0; part < 1;)
	{
		streamFrontendGlob.imageTouchBits[66 * streamFrontendGlob.activeImageTouchBits][imagePartIndex >> 5] |= 1 << (imagePartIndex & 0x1F);
		part++;
		imagePartIndex++;
	}
}

bool R_StreamTouchImageAndCheck(GfxImage *image, int level)
{
	ASSERT(level >= 0 && level < 1, "level doesn't index MAX_IMAGE_STREAMED_PARTS");

	if (!image->streaming)
		return true;

	int imagePartIndex = DB_GetImageIndex(image);
	int levelPartIndex = imagePartIndex - ((level > 0) ? 0 : level);

	for (int part = 0; part < 1;)
	{
		streamFrontendGlob.imageTouchBits[66 * streamFrontendGlob.activeImageTouchBits][imagePartIndex >> 5] |= 1 << (imagePartIndex & 0x1F);
		++part;
		++imagePartIndex;
	}

	return (streamFrontendGlob.imageUseBits[levelPartIndex >> 5] & (1 << (levelPartIndex & 0x1F))) != 0;
}

bool R_StreamImageCheck(GfxImage *image, int level)
{
	ASSERT(level >= 0 && level < 1, "level doesn't index MAX_IMAGE_STREAMED_PARTS");

	if (!image->streaming)
		return true;

	int v4 = 0;
	if (level > 0)
		v4 = 0;
	else
		v4 = level;
	int v3 = DB_GetImageIndex(image);

	return (streamFrontendGlob.imageUseBits[(signed int)(v3 - v4) >> 5] & (1 << ((v3 - v4) & 0x1F))) != 0;
}

void R_Stream_ResetHintEntities()
{
	unk_ADD7080 = 0;
	memset(s_streamHints, 0, 0x80u);
}

void R_StreamInit()
{
	R_StreamAlloc_InitTempImages();

	streamFrontendGlob.frame = 0;
	streamFrontendGlob.queryInProgress = 0;
	streamFrontendGlob.queryClient = -1;

	for (int i = 0; i < 10; i++)
		R_Stream_InvalidateRequest(&s_pendingRequests[i]);

	memset(streamFrontendGlob.imageInSortedListBits, 0, sizeof(streamFrontendGlob.imageInSortedListBits));
	memset(s_preventMaterials, 0, sizeof(s_preventMaterials));

	streamFrontendGlob.sortedImageCount = 0;
	streamFrontendGlob.diskOrderImagesNeedSorting = true;
	streamIsInitialized = true;
}

void R_StreamShutdown()
{
	R_StreamSetDefaultConfig(true);

	memset(s_preventMaterials, 0, sizeof(s_preventMaterials));
	memset(streamFrontendGlob.imageInSortedListBits, 0, sizeof(streamFrontendGlob.imageInSortedListBits));

	streamFrontendGlob.sortedImageCount = 0;
	streamFrontendGlob.diskOrderImagesNeedSorting = true;
}

void R_Stream_ReleaseImage(GfxImage *image, bool lock, bool delayDirty)
{
	ASSERT(image);

	if (!image->streaming)
		return;

	if (lock)
		R_StreamAlloc_Lock();

	int freedSize;
	char *memory = R_StreamAlloc_FreeImage(image, 0, delayDirty, &freedSize);

	if (freedSize > 0)
	{
		ASSERT(memory);
		BG_EvalVehicleName();
	}

	int imageIndex = DB_GetImageIndex(image);
	int imagePartIndex = imageIndex;

	for (int part = 0; part < 1; part++)
	{
		streamFrontendGlob.imageInitialBits[(signed int)(imagePartIndex + part) >> 5] &= ~(1 << ((imagePartIndex + part) & 0x1F));
		streamFrontendGlob.imageForceBits[(signed int)(imagePartIndex + part) >> 5] &= ~(1 << ((imagePartIndex + part) & 0x1F));
	}

	streamFrontendGlob.diskOrderImagesNeedSorting = true;

	if (lock)
		R_StreamAlloc_Unlock();
}

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
	streamFrontendGlob.calculateTotalBytesWanted = 0;
	Sys_EnterCriticalSection(CRITSECT_STREAM_FORCE_LOAD_COMMAND);

	unk_ADD7090 = 0;

	for (int i = 0; i < 4; i++)
		s_forcedLoadEntities[i] = 0;

	Sys_LeaveCriticalSection(CRITSECT_STREAM_FORCE_LOAD_COMMAND);
	R_StreamPopDisable();
	Sys_LeaveCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);
}

void R_Stream_Sync()
{
	ASSERT(streamIsInitialized);
	ASSERT(!R_StreamIsEnabled());

	if (streamFrontendGlob.queryInProgress)
		R_StreamUpdate_EndQuery();

	ASSERT(!streamFrontendGlob.queryInProgress);

	R_StreamAlloc_Lock();
	for (int i = 0; i < 10; i++)
	{
		pendingRequest *request = &s_pendingRequests[i];

		if (s_pendingRequests[i].status != 1 && request->status != 2)
		{
			if (request->status == 7)
			{
				int v1 = DB_GetImageIndex(request->image);
				streamFrontendGlob.imageLoading[v1 >> 5] &= ~(1 << (v1 & 0x1F));
				Z_VirtualFree(request->buffer, 20);
				R_Stream_InvalidateRequest(request);
			}
			else if (request->status == 3)
			{
				while (s_pendingRequests[i].status < 4 || s_pendingRequests[i].status > 7)
					NET_Sleep(1);

				int v2 = DB_GetImageIndex(request->image);
				streamFrontendGlob.imageLoading[v2 >> 5] &= ~(1 << (v2 & 0x1F));
				Z_VirtualFree(request->buffer, 20);
				R_Stream_InvalidateRequest(request);
			}
		}
		else
		{
			int imageIndex = DB_GetImageIndex(request->image);
			streamFrontendGlob.imageLoading[imageIndex >> 5] &= ~(1 << (imageIndex & 0x1F));
			R_Stream_InvalidateRequest(request);
		}
	}
	R_StreamAlloc_Unlock();
}

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

void R_StreamUpdate_Idle()
{
	if (DB_FinishedLoadingAssets())
	{
		if (R_StreamUpdate(nullptr))
			R_StreamUpdate_EndQuery();
	}
}

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

		NET_Sleep(10);
	}

	R_EndRemoteScreenUpdate(pumpfunc);
}

bool R_StreamUpdate(const float *viewPos)
{
	bool updateCalled = false;

	if (r_streamClear->current.enabled || r_stream->modified)
	{
		R_StreamSyncThenFlush(1);
		Dvar_ClearModified(r_streamSize);
		Dvar_ClearModified(r_stream);
		Dvar_SetBool(r_streamClear, 0);
	}

	if (r_streamSize->modified)
	{
		R_StreamSyncThenFlush(0);
		Dvar_ClearModified(r_streamSize);
	}

	if (r_streamLowDetail->modified)
	{
		if (r_streamLowDetail->current.enabled)
			BG_EvalVehicleName();
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

void R_Stream_AddImagePartImportance(int imagePartIndex, float importance)
{
	float v2; // [sp+0h] [bp-8h]@5

	ASSERT(imagePartIndex >= 0 && imagePartIndex < 0x1080, "imagePartIndex doesn't index TOTAL_IMAGE_PARTS");

	if ((float)(importance - streamFrontendGlob.imageImportance[imagePartIndex]) < 0.0f)
		v2 = streamFrontendGlob.imageImportance[imagePartIndex];
	else
		v2 = importance;

	streamFrontendGlob.imageImportance[imagePartIndex] = v2;

	if (!(streamFrontendGlob.imageImportanceBits[imagePartIndex >> 5] & (1 << (imagePartIndex & 0x1F))))
	{
		streamFrontendGlob.imageImportanceBits[imagePartIndex >> 5] |= 1 << (imagePartIndex & 0x1F);

		if (!(streamFrontendGlob.imageInSortedListBits[imagePartIndex >> 5] & (1 << (imagePartIndex & 0x1F))))
		{
			streamFrontendGlob.imageInSortedListBits[imagePartIndex >> 5] |= 1 << (imagePartIndex & 0x1F);
			streamFrontendGlob.sortedImages[streamFrontendGlob.sortedImageCount++] = (signed __int16)imagePartIndex;
		}
	}
}

void importance_swap_func(void **a, void **b)
{
	void *temp = *a;
	*a = *b;
	*b = temp;
}

bool __cdecl importance_compare_func(void *a, void *b)
{
	return SLODWORD(streamFrontendGlob.imageImportance[(_DWORD)a]) > SLODWORD(streamFrontendGlob.imageImportance[(_DWORD)b]);
}

void __cdecl importance_merge_sort(void **list, const int list_count)
{
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
	else if (list_count == 2 && importance_compare_func(list[1], *list))
	{
		importance_swap_func(list, list + 1);
	}
}

void R_StreamUpdate_EndQuery()
{
	ASSERT(streamFrontendGlob.queryInProgress == 1);

	Sys_WaitWorkerCmdInternal(&r_stream_sortWorkerCmd);

	streamFrontendGlob.queryInProgress = 0;

	R_StreamAlloc_Lock();
	R_StreamUpdate_EndQuery_Internal();
	R_StreamAlloc_Unlock();
}

DWORD R_StreamUpdate_EndQuery_Internal()
{
	// todo
}

bool R_StreamRequestImageAllocation(pendingRequest *request, GfxImage *image, bool highMip, int imagePart, float importance)
{
	ASSERT(image);
	ASSERT(request);
	ASSERT(image->streaming != GFX_TEMP_STREAMING);

	ASSERT(image->streaming);

	if (!image->streaming)
		return false;

	ASSERT(request->image == nullptr);
	ASSERT(request->status == STREAM_STATUS_INVALID);

	request->image = image;
	request->imagePart = imagePart;
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
				"-STREAM-allocation complete. bytes=%d,image=%s,imagePart=%d,importance=%f\n",
				request->bufferSize,
				image->name,
				imagePart,
				importance),
			0);
	}

	int imageIndex = DB_GetImageIndex(image);

	ASSERT(!(streamFrontendGlob.imageLoading[BIT_INDEX_32(imageIndex)] & BIT_MASK_32(imageIndex)));

	streamFrontendGlob.imageLoading[imageIndex >> 5] |= 1 << (imageIndex & 0x1F);
	request->status = 1;
	request->id[0] = -1;
	return true;
}

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

		for (int i = 0; i < 10; i++)
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

			ASSERT(streamFrontendGlob.imageUseBits[BIT_INDEX_32(unloadImagePartIndex)] & BIT_MASK_32(unloadImagePartIndex));

			if (R_StreamRequestImageAllocation(
				unloadRequest,
				unloadImage,
				0,
				0,
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

	streamFrontendGlob.imageLoading[imageIndex >> 5] &= ~(1 << (imageIndex & 0x1F));
	R_Stream_InvalidateRequest(request);
	return false;
}

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
		streamFrontendGlob.diskOrder = 1;

		R_StreamUpdate_AddForcedImages(
			streamFrontendGlob.forcedImageImportance,
			streamFrontendGlob.touchedImageImportance);

		R_StreamUpdate_AddInitialImages(streamFrontendGlob.initialImageImportance);

		StreamSortCmd sortCmd;
		sortCmd.frontend = &streamFrontendGlob;
		sortCmd.diskOrder = 1;
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
	Sys_AddWorkerCmdInternal(&r_stream_updateWorkerCmd, &updateCmd, 0);

	for (int hint = 0; hint < 4; hint++)
	{
		if (s_streamHints[hint].importance > 0.0f)
		{
			updateCmd.distanceScale[1] = 1.0f / s_streamHints[hint].importance;
			updateCmd.distanceScale[0] = updateCmd.distanceScale[1];
			updateCmd.viewPos[0] = s_streamHints[hint].origin[0];
			updateCmd.viewPos[1] = s_streamHints[hint].origin[1];
			updateCmd.viewPos[2] = s_streamHints[hint].origin[2];
			Sys_AddWorkerCmdInternal(&r_stream_updateWorkerCmd, &updateCmd, 0);
		}
	}

	Sys_LeaveCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);
	return true;
}

void R_StreamUpdate_AddInitialImages(float importance)
{
	for (int imagePartIndex = 0; imagePartIndex < 4224; imagePartIndex++)
	{
		if (streamFrontendGlob.imageInitialBits[imagePartIndex >> 5] & (1 << (imagePartIndex & 0x1F)))
			R_Stream_AddImagePartImportance(imagePartIndex, importance);
	}
}

void __cdecl R_StreamUpdate_AddForcedImages(float forceImportance, float touchImportance)
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

		if (!_BitScanReverse(&v2, forceBits))
			v2 = `CountLeadingZeros'::`2': : notFound;
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

			if (!_BitScanReverse((unsigned int *)&v5, forceBits))
				v5 = `CountLeadingZeros'::`2': : notFound;
			v3 = v5 ^ 0x1F;
		}

		if (!_BitScanReverse((unsigned int *)&v3, useBits))
			v3 = `CountLeadingZeros'::`2': : notFound;

		for (int bitIndex = 31 - (v3 ^ 0x1F); bitIndex >= 0; bitIndex = 31 - (v6 ^ 0x1F))
		{
			ASSERT(useBits & BIT_MASK_32(bitIndex));

			useBits &= ~(1 << (bitIndex & 0x1F));

			R_Stream_AddImagePartImportance(bitIndex + 32 * index, 0.0);

			if (!_BitScanReverse((unsigned int *)&v6, useBits))
				v6 = `CountLeadingZeros'::`2': : notFound;
		}
	}

	memset(streamFrontendGlob.imageTouchBits[66 * (streamFrontendGlob.activeImageTouchBits ^ 1)], 0, 0x210u);
}

void R_StreamUpdatePreventedMaterials()
{
	memset(streamFrontendGlob.materialPreventBits, 0, sizeof(streamFrontendGlob.materialPreventBits));

	for (int i = 0; i < 32; i++)
	{
		if (s_preventMaterials[i])
		{
			int materialIndex = DB_GetMaterialIndex(s_preventMaterials[i]);
			streamFrontendGlob.materialPreventBits[materialIndex >> 5] |= 1 << (materialIndex & 0x1F);
		}
	}
}

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

	R_CheckHighmipAabbs_r(rgp, 0, mins, maxs);
}

bool R_StreamUpdate_TryBeginQuery()
{
	if (!streamFrontendGlob.queryInProgress && R_StreamIsEnabled())
	{
		if (streamFrontendGlob.outputImageList)
		{
			R_ImageList_Output();
			streamFrontendGlob.outputImageList = 0;
		}

		memset(streamFrontendGlob.materialImportance, 0, 0x4000u);
		memset(streamFrontendGlob.materialImportanceBits, 0, 0x200u);
		memset(streamFrontendGlob.modelDistance, 0, 0xFA0u);
		memset(streamFrontendGlob.modelDistanceBits, 0, 0x80u);
		memset(streamFrontendGlob.imageImportance, 0, 0x4200u);
		memset(streamFrontendGlob.imageImportanceBits, 0, 0x210u);
		memset(streamFrontendGlob.dynamicModelDistance, 0, 0xFA0u);
		memset(streamFrontendGlob.dynamicModelDistanceBits, 0, 0x80u);
		return true;
	}

	return false;
}

void R_StreamUpdateTouchedModels()
{
	for (modelIndex = 0; modelIndex < 0x3E8; modelIndex++)
	{
		if (streamFrontendGlob.modelTouchBits[modelIndex >> 5] & (1 << (modelIndex & 0x1F)))
		{
			XModel *model = DB_GetXModelAtIndex(modelIndex);
			ASSERT(model);

			R_StreamUpdateForXModelTouched(model);
		}
	}
}

void R_StreamUpdateForXModelTouched(XModel *model)
{
	if (!model)
		return;

	for (int lod = 0; lod < 4; lod++)
	{
		XSurface *surfaces = nullptr;
		Material **material = nullptr;

		int surfCount = XModelGetSurfaces(model, (XSurface **)&surfaces, lod);
		material = (Material **)XModelGetSkins(model, lod);

		for (int surfIter = 0; surfIter != surfCount;)
		{
			R_StreamTouchMaterial(*material);
			surfIter++;
			material++;
		}
	}
}

void R_StreamUpdateForcedModels(unsigned int frame)
{
	int surfCountPrevLods = 0;
	Sys_EnterCriticalSection(CRITSECT_STREAM_FORCE_LOAD_COMMAND);

	for (i = 0; i < unk_ADD7090; i++)
	{
		int modelCount = DObjGetNumModels(s_forcedLoadEntities[i]);
		float distSq = streamFrontendGlob.forcedImageImportance;

		for (int modelIter = 0; modelIter != modelCount; modelIter++)
		{
			XModel *model = DObjGetModel(s_forcedLoadEntities[i], modelIter);

			for (lod = 0; lod < 4; lod++)
			{
				XSurface *surfaces = nullptr;
				Material **material = nullptr;

				int surfCount = XModelGetSurfaces(model, (XSurface **)&surfaces, lod);
				material = (Material **)XModelGetSkins(model, lod);

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

void R_StreamTouchImagesFromMaterial(Material *remoteMaterial, float importance)
{
	int textureCount = remoteMaterial->textureCount;

	if (!remoteMaterial->textureCount)
		return;

	for (int textureIter = 0; textureIter != textureCount; textureIter++)
	{
		MaterialTextureDef *texDef = &remoteMaterial->textureTable[textureIter];
		ASSERT(texDef);

		if (texDef->semantic == 11)
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
		Sys_AddWorkerCmdInternal(&r_stream_updateWorkerCmd, &updateCmd, 0);
	}

	float distanceScale[2];
	distanceScale[1] = 1.0f;
	distanceScale[0] = r_streamHiddenPush->current.value;
	R_StreamUpdateDynamicModels(viewPos, maxDistSq, streamFrontendGlob.frame, distanceScale);
	streamFrontendGlob.queryClient++;

	Sys_LeaveCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);
}

void R_StreamUpdateDynamicModels(const float *viewPos, float maxDistSq, unsigned int frame, float *distanceScale)
{
	// Doesn't reference streamFrontendGlob. Not re-implemented.
	__debugbreak();
}

void R_StreamUpdateForBModel(const float *viewPos, unsigned int frame, unsigned int surfId, GfxBrushModel *bmodel, const float *origin, float maxDistSq, Material *altMaterial, bool isVisible, float *distanceScale)
{
	// Doesn't reference streamFrontendGlob. Not re-implemented.
	__debugbreak();
}

void R_StreamUpdate_AddDynamicXModelDistance(XModel *model, const float *viewPos, const float *origin, const float scale, bool visible, float *distanceScale)
{
	int modelIndex = DB_GetXModelIndex(model);
	float distSq = (Vec3DistanceSq(viewPos, origin) / scale) * distanceScale[visible];

	if (streamFrontendGlob.dynamicModelDistanceBits[modelIndex >> 5] & (1 << (modelIndex & 0x1F)))
	{
		float v6; // [sp+0h] [bp-1Ch]@3
		if ((float)(distSq - streamFrontendGlob.dynamicModelDistance[modelIndex]) < 0.0f)
			v6 = distSq;
		else
			v6 = streamFrontendGlob.dynamicModelDistance[modelIndex];
		streamFrontendGlob.dynamicModelDistance[modelIndex] = v6;
	}
	else
	{
		streamFrontendGlob.dynamicModelDistance[modelIndex] = distSq;
		streamFrontendGlob.dynamicModelDistanceBits[modelIndex >> 5] |= 1 << (modelIndex & 0x1F);
	}
}

void R_StreamUpdate_End()
{
	ASSERT(streamFrontendGlob.queryClient > 0);

	Sys_EnterCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);

	StreamCombineCmd combineCmd;
	combineCmd.frontend = &streamFrontendGlob;
	Sys_AddWorkerCmdInternal(&r_stream_combineWorkerCmd, &combineCmd, 0);

	StreamSortCmd sortCmd;
	sortCmd.frontend = &streamFrontendGlob;
	sortCmd.diskOrder = streamFrontendGlob.diskOrder;
	Sys_AddWorkerCmdInternal(&r_stream_sortWorkerCmd, &sortCmd, 0);

	streamFrontendGlob.queryClient = -1;
	Sys_LeaveCriticalSection(CRITSECT_STREAM_SYNC_COMMAND);
}

void R_Stream_UpdateStaticModelsCmd(void *data)
{
	// Doesn't reference streamFrontendGlob. Not re-implemented.
	__debugbreak();
}

void R_StreamUpdateStaticModel(int staticModelIndex, const float *viewPos, float maxDistSq, float *distanceScale)
{
	// Doesn't reference streamFrontendGlob. Not re-implemented.
	__debugbreak();
}

void R_StreamUpdate_AddXModelDistance(XModel *model, const float *viewPos, const float *origin, const float scale, bool visible, float *distanceScale)
{
	int modelIndex = DB_GetXModelIndex(model);
	float distSq = (Vec3DistanceSq(viewPos, origin) / scale) * distanceScale[visible];

	if (streamFrontendGlob.modelDistanceBits[modelIndex >> 5] & (1 << (modelIndex & 0x1F)))
	{
		float v6; // [sp+0h] [bp-1Ch]@3
		if ((float)(distSq - streamFrontendGlob.modelDistance[modelIndex]) < 0.0)
			v6 = distSq;
		else
			v6 = streamFrontendGlob.modelDistance[modelIndex];
		streamFrontendGlob.modelDistance[modelIndex] = v6;
	}
	else
	{
		streamFrontendGlob.modelDistance[modelIndex] = distSq;
		streamFrontendGlob.modelDistanceBits[modelIndex >> 5] |= 1 << (modelIndex & 0x1F);
	}
}

void R_Stream_UpdateStaticSurfacesCmd(void *data)
{
	// Doesn't reference streamFrontendGlob. Not re-implemented.
	__debugbreak();
}

void R_StreamUpdateWorldSurface(int surfId, const float *viewPos, float maxDistSq, float *distanceScale)
{
	ASSERT(surfId >= 0 && surfId < rgp.world->surfaceCount);

	GfxSurface *surface = &rgp->dpvs.surfaces[surfId];
	bool isVisible = rgp->dpvs.surfaceVisDataCameraSaved[surfId] != 0;

	if (!dword_A868184 || isVisible)
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

		float v4;
		if ((float)(distSq.importance - streamFrontendGlob.materialImportance[materialIndex]) < 0.0)
			v4 = streamFrontendGlob.materialImportance[materialIndex];
		else
			v4 = distSq.importance;

		streamFrontendGlob.materialImportance[materialIndex] = v4;
	}
}

void __cdecl MultiplePointDistSqFromBounds(distance_data *distances, const float *v, const float *mip0mins, const float *mip0maxs, float himipRadiusSq, float distanceScale)
{
	float4 v6; // ST10_16@10
	float4 v7; // ST00_16@10

	ASSERT(((uint)(&s_viewPos.v[0]) & 15) == 0);
	ASSERT(((uint)((void*)mip0mins) & 15) == 0);
	ASSERT(((uint)((void*)mip0maxs) & 15) == 0);

	*(_QWORD *)&v6.v[0] = *(_QWORD *)mip0maxs;
	*(_QWORD *)&v6.unitVec[2].packed = *((_QWORD *)mip0maxs + 1);
	*(_QWORD *)&v7.v[0] = *(_QWORD *)mip0mins;
	*(_QWORD *)&v7.unitVec[2].packed = *((_QWORD *)mip0mins + 1);
	distances->distanceForHimip = FastPointDistSqFromBounds(v7, v6) * distanceScale;
	distances->importance = himipRadiusSq / (float)(distances->distanceForHimip + 100.0);
}

float FastPointDistSqFromBounds(float4 mins, float4 maxs)
{
	float v3; // [sp+4h] [bp-120h]@17
	float v4; // [sp+8h] [bp-11Ch]@14
	float v5; // [sp+Ch] [bp-118h]@11
	float v6; // [sp+14h] [bp-110h]@8
	float v7; // [sp+18h] [bp-10Ch]@5
	float v8; // [sp+1Ch] [bp-108h]@2

	if ((float)(s_viewPos.v[0] - mins.v[0]) < 0.0)
		v8 = mins.v[0];
	else
		v8 = s_viewPos.v[0];
	if ((float)(s_viewPos.v[1] - mins.v[1]) < 0.0)
		v7 = mins.v[1];
	else
		v7 = s_viewPos.v[1];
	if ((float)(s_viewPos.v[2] - mins.v[2]) < 0.0)
		v6 = mins.v[2];
	else
		v6 = s_viewPos.v[2];
	if ((float)(maxs.v[0] - v8) < 0.0)
		v5 = maxs.v[0];
	else
		v5 = v8;
	if ((float)(maxs.v[1] - v7) < 0.0)
		v4 = maxs.v[1];
	else
		v4 = v7;
	if ((float)(maxs.v[2] - v6) < 0.0)
		v3 = maxs.v[2];
	else
		v3 = v6;
	return (float)((float)((float)(v5 - s_viewPos.v[0]) * (float)(v5 - s_viewPos.v[0]))
		+ (float)((float)(v4 - s_viewPos.v[1]) * (float)(v4 - s_viewPos.v[1])))
		+ (float)((float)(v3 - s_viewPos.v[2]) * (float)(v3 - s_viewPos.v[2]));
}

void R_Stream_SortCmd(void *data)
{
	R_StreamUpdate_EndQuerySort(*((bool *)data + 4));
}

void R_StreamUpdate_EndQuerySort(bool diskOrder)
{
	for (int index = 0; index < streamFrontendGlob.sortedImageCount;)
	{
		int imagePartIndex = streamFrontendGlob.sortedImages[index];

		ASSERT(streamFrontendGlob.imageInSortedListBits[BIT_INDEX_32(imagePartIndex)] & BIT_MASK_32(imagePartIndex));

		if (streamFrontendGlob.imageImportanceBits[imagePartIndex >> 5] & (1 << (imagePartIndex & 0x1F)))
		{
			index++;
		}
		else
		{
			streamFrontendGlob.sortedImages[index] = streamFrontendGlob.sortedImages[--streamFrontendGlob.sortedImageCount];
			streamFrontendGlob.imageInSortedListBits[imagePartIndex >> 5] &= ~(1 << (imagePartIndex & 0x1F));
		}
	}

	if (diskOrder)
	{
		if (streamFrontendGlob.diskOrderImagesNeedSorting || streamFrontendGlob.forceDiskOrder)
		{
			std::_Sort<int *, int, importance_and_offset_pred>(
				streamFrontendGlob.sortedImages,
				&streamFrontendGlob.sortedImages[streamFrontendGlob.sortedImageCount],
				4 * streamFrontendGlob.sortedImageCount >> 2,
				0);
			streamFrontendGlob.diskOrderImagesNeedSorting = 0;
		}
	}
	else
	{
		importance_merge_sort((void **)streamFrontendGlob.sortedImages, streamFrontendGlob.sortedImageCount);
	}
}

void R_Stream_CombineCmd(void *data)
{
	R_StreamUpdate_CombineImportance();
}

void R_StreamUpdate_CombineImportance()
{
	signed int mask; // ST110_4@3
	__int128 v1; // xmm0@29
	DWORD result; // eax@36
	float v4; // [sp+4h] [bp-104h]@8
	unsigned int materialIndex; // [sp+F0h] [bp-18h]@26
	unsigned int staticBit; // [sp+F4h] [bp-14h]@3
	unsigned int dynamicBit; // [sp+F8h] [bp-10h]@3
	unsigned int modelIndex; // [sp+104h] [bp-4h]@1

	for (int modelIndex = 0; modelIndex < 1000; modelIndex++)
	{
		mask = 1 << (modelIndex & 0x1F);
		staticBit = mask & streamFrontendGlob.modelDistanceBits[modelIndex >> 5];
		dynamicBit = mask & streamFrontendGlob.dynamicModelDistanceBits[modelIndex >> 5];

		if (dynamicBit & staticBit)
		{
			XModel *model = DB_GetXModelAtIndex(modelIndex);
			ASSERT(model);

			if ((float)(streamFrontendGlob.dynamicModelDistance[modelIndex] - streamFrontendGlob.modelDistance[modelIndex]) < 0.0)
				v4 = streamFrontendGlob.dynamicModelDistance[modelIndex];
			else
				v4 = streamFrontendGlob.modelDistance[modelIndex];

			R_StreamUpdateForXModel(model, v4);
		}
		else if (staticBit)
		{
			XModel *model = DB_GetXModelAtIndex(modelIndex);
			ASSERT(model);

			R_StreamUpdateForXModel(model, streamFrontendGlob.modelDistance[modelIndex]);
		}
		else if (dynamicBit)
		{
			XModel *model = DB_GetXModelAtIndex(modelIndex);
			ASSERT(model);

			R_StreamUpdateForXModel(model, streamFrontendGlob.dynamicModelDistance[modelIndex]);
		}
	}

	for (int materialIndex = 0; materialIndex < 4096; materialIndex++)
	{
		v1 = LODWORD(streamFrontendGlob.materialImportance[materialIndex]);
		__asm { lahf }
		if (__SETP__(_AH & 0x44, 0)
			&& !(streamFrontendGlob.materialPreventBits[materialIndex >> 5] & (1 << (materialIndex & 0x1F))))
		{
			Material *material = DB_GetMaterialAtIndex(materialIndex);
			ASSERT(material);

			R_StreamTouchImagesFromMaterial(material, streamFrontendGlob.materialImportance[materialIndex]);
		}
	}
}

void R_StreamUpdateForXModel(XModel *remoteModel, float distSq)
{
	__int128 v2; // xmm0@5
	float v4; // [sp+0h] [bp-40h]@17
	float v5; // [sp+4h] [bp-3Ch]@8
	float v6; // [sp+10h] [bp-30h]@7
	int bounds; // [sp+28h] [bp-18h]@5
	float importance; // [sp+2Ch] [bp-14h]@10
	signed int surf; // [sp+34h] [bp-Ch]@1

	float distNotSq = fsqrt(distSq);
	Material **materialHandles = remoteModel->materialHandles;
	XModelHighMipBounds *highMipBounds = remoteModel->streamInfo.highMipBounds;

	for (surf = 0; surf < remoteModel->numsurfs; ++surf)
	{
		if (!materialHandles[surf]->textureCount)
			continue;

		bounds = (int)&highMipBounds[surf];
		v2 = LODWORD(highMipBounds[surf].himipRadiusSq);
		__asm { lahf }
		if (__SETP__(_AH & 0x44, 0))
		{
			v6 = distNotSq - Abs(bounds->center);

			if ((float)((float)(v6 * v6) - 0.0) < 0.0)
				v5 = FLOAT_0_0;
			else
				v5 = v6 * v6;

			float importance = bounds->himipRadiusSq / (float)(v5 + 100.0);
			int materialIndex = DB_GetMaterialIndex(materialHandles[surf]);

			ASSERT(materialIndex >= 0);
			ASSERT(materialIndex < MAX_MATERIAL_POOL_SIZE);

			if ((float)(importance - streamFrontendGlob.materialImportance[materialIndex]) < 0.0)
				v4 = streamFrontendGlob.materialImportance[materialIndex];
			else
				v4 = importance;

			streamFrontendGlob.materialImportance[materialIndex] = v4;
		}
	}
}

void R_StreamUpdateStatic(const float *viewPos, float maxDistSq, float *distanceScale)
{
	ASSERT(viewPos);

	// Doesn't reference streamFrontendGlob. Not re-implemented.
	__debugbreak();
}

void R_StreamUpdateAabbNode_r_0_(int aabbTreeNode, const float *viewPos, float maxDistSq, float *distanceScale)
{
	// Doesn't reference streamFrontendGlob. Not re-implemented.
	__debugbreak();
}
#endif