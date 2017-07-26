#pragma once

#if 0
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

struct __declspec(align(4)) StreamFrontendGlob
{
	void *mainBuffer;
	int mainBufferSize;
	void *extraBuffer;
	int extraBufferSize;
	unsigned int frame;
	char _pad0[108];
	float materialImportance[4096];
	unsigned int materialImportanceBits[128];
	unsigned int materialTouchBits[128];
	unsigned int materialPreventBits[128];
	float modelDistance[1000];
	char _pad1[96];
	unsigned int modelDistanceBits[32];
	float dynamicModelDistance[1000];
	char _pad2[96];
	unsigned int dynamicModelDistanceBits[32];
	unsigned int modelTouchBits[32];
	unsigned int imageInSortedListBits[132];
	char _pad3[112];
	float imageImportance[4224];
	unsigned int imageImportanceBits[132];
	char _pad4[112];
	float dynamicImageImportance[4224];
	unsigned int dynamicImageImportanceBits[132];
	char _pad5[112];
	unsigned int dummy;
	unsigned int imageLoading[132];
	unsigned int imageUseBits[132];
	unsigned int imageForceBits[132];
	unsigned int imageInitialBits[132];
	unsigned int imageTouchBits[132][2];
	int activeImageTouchBits;
	float touchedImageImportance;
	float initialImageImportance;
	float forcedImageImportance;
	bool imageInitialBitsSet;
	int initialLoadAllocFailures;
	bool preloadCancelled;
	bool diskOrderImagesNeedSorting;
	int sortedImages[4224];
	int sortedImageCount;
	bool calculateTotalBytesWanted;
	int totalBytesWanted;
	volatile int queryClient;
	volatile int queryInProgress;
	bool diskOrder;
	bool forceDiskOrder;
	bool outputImageList;
	bool ignoreMainView;
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

struct streamerHintInfo
{
	int entNum;
	float origin[3];
	float angles[3];
	float importance;
};

struct distance_data
{
	float importance;
	float distanceForHimip;
};

extern StreamFrontendGlob streamFrontendGlob;

bool R_StreamIsEnabled();
void R_StreamPushDisable();
void R_StreamPopDisable();
void R_StreamSetInitData(int buffer_size);
void R_StreamSetDefaultConfig(bool clear);
void R_StreamSetUIConfig(bool clear);
float R_Stream_GetProgress();
void R_Stream_InvalidateRequest(pendingRequest *request);
bool R_StreamRequestImageAllocation(pendingRequest *request, GfxImage *image, bool highMip, int imagePart, float importance);
void R_StreamUpdate_ReadTextures();
bool R_StreamRequestImageRead(pendingRequest *request);
bool R_StreamUpdate_ProcessFileCallbacks();
void R_StreamUpdate_SetupInitialImageList();
void R_StreamUpdate_AddInitialImages(float importance);
void R_StreamUpdate_AddForcedImages(float forceImportance, float touchImportance);
void R_Stream_ForceLoadImage(GfxImage *image, int part);
void R_Stream_ForceLoadMaterial(Material *material, int part);
void R_Stream_ForceLoadModel(XModel *model, int part);
bool R_StreamTouchDObjAndCheck(DObj *obj, int level);
void R_StreamTouchMaterial(Material *material);
bool R_StreamTouchMaterialAndCheck(Material *material, int level);
void R_StreamTouchImage(GfxImage *image);
bool R_StreamTouchImageAndCheck(GfxImage *image, int level);
bool R_StreamImageCheck(GfxImage *image, int level);
void R_Stream_ResetHintEntities();
void R_StreamUpdatePreventedMaterials();
void R_StreamInit();
void R_StreamShutdown();
void R_Stream_ReleaseImage(GfxImage *image, bool lock, bool delayDirty);
void R_Stream_Sync();
void R_StreamSyncThenFlush(bool flushAll);
void R_StreamPushSyncDisable();
void R_StreamPopSyncDisable();
void R_CheckHighmipAabbs();
bool R_StreamUpdate_TryBeginQuery();
void R_StreamUpdate_Idle();
void R_StreamUpdate_CompletePreload(void(__cdecl *pumpfunc)());
bool R_StreamUpdate(const float *viewPos);
void R_Stream_AddImagePartImportance(int imagePartIndex, float importance);
void R_StreamTouchImagesFromMaterial(Material *remoteMaterial, float importance);
float FastPointDistSqFromBounds(float4 mins, float4 maxs);
void MultiplePointDistSqFromBounds(distance_data *distances, const float *v, const float *mip0mins, const float *mip0maxs, float himipRadiusSq, float distanceScale);
void R_StreamUpdateForXModel(XModel *remoteModel, float distSq);
void R_StreamUpdateForXModelTouched(XModel *model);
void R_StreamUpdateTouchedModels();
void R_StreamUpdateForBModel(const float *viewPos, unsigned int frame, unsigned int surfId, GfxBrushModel *bmodel, const float *origin, float maxDistSq, Material *altMaterial, bool isVisible, float *distanceScale);
void R_StreamUpdate_AddXModelDistance(XModel *model, const float *viewPos, const float *origin, const float scale, bool visible, float *distanceScale);
void R_StreamUpdate_AddDynamicXModelDistance(XModel *model, const float *viewPos, const float *origin, const float scale, bool visible, float *distanceScale);
void R_StreamUpdateDynamicModels(const float *viewPos, float maxDistSq, unsigned int frame, float *distanceScale);
void R_StreamUpdateStaticModel(int staticModelIndex, const float *viewPos, float maxDistSq, float *distanceScale);
void R_StreamUpdateWorldSurface(int surfId, const float *viewPos, float maxDistSq, float *distanceScale);
void R_StreamUpdate_CombineImportance();
void R_StreamUpdateAabbNode_r_0_(int aabbTreeNode, const float *viewPos, float maxDistSq, float *distanceScale);
void R_StreamUpdateStatic(const float *viewPos, float maxDistSq, float *distanceScale);
void importance_swap_func(void **a, void **b);
bool importance_compare_func(void *a, void *b);
void importance_merge_sort(void **list, const int list_count);
void R_StreamUpdate_EndQuerySort(bool diskOrder);
void R_StreamUpdateForcedModels(unsigned int frame);
DWORD R_StreamUpdate_EndQuery_Internal();
void R_StreamUpdate_EndQuery();
bool R_StreamUpdate_FindImageAndOptimize(const float *viewPos);
void R_StreamUpdatePerClient(const float *viewPos);
void R_StreamUpdate_End();
void R_Stream_UpdateStaticModelsCmd(void *data);
void R_Stream_UpdateStaticSurfacesCmd(void *data);
void R_Stream_SortCmd(void *data);
void R_Stream_CombineCmd(void *data);
#endif