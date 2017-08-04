#pragma once

#define STREAM_MAX_IMAGE_PARTS 1
#define STREAM_MAX_REQUESTS 10

#define STREAM_MAX_MATERIALS 4096
#define STREAM_MAX_MATERIAL_BITS (STREAM_MAX_MATERIALS / 32)

#define STREAM_MAX_MODELS 2048
#define STREAM_MAX_MODEL_BITS (STREAM_MAX_MODELS / 32)

#define STREAM_MAX_IMAGES 6048
#define STREAM_MAX_IMAGE_BITS (STREAM_MAX_IMAGES / 32)

enum
{
	GFX_NOT_STREAMING = 0x0,
	GFX_STREAMING = 0x1,
	GFX_MANUAL_STREAMING = 0x2,
	GFX_TEMP_STREAMING = 0x3,
};

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

struct  __declspec(align(128)) StreamFrontendGlob
{
	void *mainBuffer;
	int mainBufferSize;
	void *extraBuffer;
	int extraBufferSize;
	unsigned int frame;
	char _pad0[108];
	float materialImportance[STREAM_MAX_MATERIALS];
	unsigned int materialImportanceBits[STREAM_MAX_MATERIAL_BITS];
	unsigned int materialTouchBits[STREAM_MAX_MATERIAL_BITS];
	unsigned int materialPreventBits[STREAM_MAX_MATERIAL_BITS];
	float modelDistance[STREAM_MAX_MODELS];
	char _pad1[96];
	unsigned int modelDistanceBits[STREAM_MAX_MODEL_BITS];
	float dynamicModelDistance[STREAM_MAX_MODELS];
	char _pad2[96];
	unsigned int dynamicModelDistanceBits[STREAM_MAX_MODEL_BITS];
	unsigned int modelTouchBits[STREAM_MAX_MODEL_BITS];
	unsigned int imageInSortedListBits[STREAM_MAX_IMAGE_BITS];
	char _pad3[112];
	float imageImportance[STREAM_MAX_IMAGES];
	unsigned int imageImportanceBits[STREAM_MAX_IMAGE_BITS];
	char _pad4[0x4AC4];
	unsigned int imageLoading[STREAM_MAX_IMAGE_BITS];
	unsigned int imageUseBits[STREAM_MAX_IMAGE_BITS];
	unsigned int imageForceBits[STREAM_MAX_IMAGE_BITS];
	unsigned int imageInitialBits[STREAM_MAX_IMAGE_BITS];
	unsigned int imageTouchBits[STREAM_MAX_IMAGE_BITS][2];
	int activeImageTouchBits;
	float touchedImageImportance;
	float initialImageImportance;
	float forcedImageImportance;
	bool imageInitialBitsSet;
	int initialLoadAllocFailures;
	bool preloadCancelled;
	bool diskOrderImagesNeedSorting;
	char _pad5[0x62];
	int sortedImages[STREAM_MAX_IMAGES];
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
/*
STATIC_ASSERT_OFFSET(StreamFrontendGlob, frame, 0x10);
STATIC_ASSERT_OFFSET(StreamFrontendGlob, materialImportance, 0x80);
STATIC_ASSERT_OFFSET(StreamFrontendGlob, materialImportanceBits, 0x4080);
STATIC_ASSERT_OFFSET(StreamFrontendGlob, dynamicModelDistanceBits, 0x6700);
STATIC_ASSERT_OFFSET(StreamFrontendGlob, imageInSortedListBits, 0x6800);
STATIC_ASSERT_OFFSET(StreamFrontendGlob, imageImportance, 0x6A80);
STATIC_ASSERT_OFFSET(StreamFrontendGlob, imageImportanceBits, 0xB280);
STATIC_ASSERT_OFFSET(StreamFrontendGlob, imageLoading, 0xFF84);
STATIC_ASSERT_OFFSET(StreamFrontendGlob, imageUseBits, 0x101C4);
STATIC_ASSERT_OFFSET(StreamFrontendGlob, imageTouchBits, 0x10884);
STATIC_ASSERT_OFFSET(StreamFrontendGlob, imageInitialBitsSet, 0x10D14);
STATIC_ASSERT_OFFSET(StreamFrontendGlob, initialLoadAllocFailures, 0x10D18);
STATIC_ASSERT_OFFSET(StreamFrontendGlob, diskOrderImagesNeedSorting, 0x10D1D);
STATIC_ASSERT_OFFSET(StreamFrontendGlob, sortedImages, 0x10D80);
STATIC_ASSERT_OFFSET(StreamFrontendGlob, sortedImageCount, 0x15580);
STATIC_ASSERT_OFFSET(StreamFrontendGlob, queryClient, 0x1558C);
STATIC_ASSERT_OFFSET(StreamFrontendGlob, queryInProgress, 0x15590);
STATIC_ASSERT_OFFSET(StreamFrontendGlob, diskOrder, 0x15594);
STATIC_ASSERT_OFFSET(StreamFrontendGlob, forceDiskOrder, 0x15595);
*/

struct pendingRequest
{
	int id[STREAM_MAX_IMAGE_PARTS];
	GfxImage *image;
	int imagePart;
	char *buffer;
	int bufferSize;
	int startTime;
	float importance;
	stream_status status;
	stream_status partStatus[STREAM_MAX_IMAGE_PARTS];
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

struct StreamCombineCmd
{
	StreamFrontendGlob *frontend;
};

struct StreamSortCmd
{
	StreamFrontendGlob *frontend;
	bool diskOrder;
};

struct StreamUpdateCmd
{
	StreamFrontendGlob *frontend;
	float viewPos[3];
	float maxDistSq;
	float distanceScale[2];
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
stream_status R_StreamRequestImageAllocation(pendingRequest *request, GfxImage *image, bool highMip, float importance);
void R_StreamUpdate_ReadTextures();
bool R_StreamRequestImageRead(pendingRequest *request);
float PointDistSqFromBounds(const float *v, const float *mins, const float *maxs);
bool R_StreamUpdate_ProcessFileCallbacks();
void R_StreamUpdate_SetupInitialImageList();
void R_Stream_ForceLoadLowDetail();
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
float FastPointDistSqFromBounds(const float4& mins, const float4& maxs);
void MultiplePointDistSqFromBounds(distance_data *distances, const float *v, const float *mip0mins, const float *mip0maxs, float himipRadiusSq, float distanceScale);
void R_StreamUpdateForXModel(XModel *remoteModel, float distSq);
void R_StreamUpdateForXModelTouched(XModel *model);
void R_StreamUpdateTouchedModels();
void R_StreamUpdateForBModel(const float *viewPos, unsigned int frame, unsigned int surfId, struct GfxBrushModel *bmodel, const float *origin, float maxDistSq, Material *altMaterial, bool isVisible, float *distanceScale);
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
void R_StreamUpdate_EndQuery_Internal();
void R_StreamUpdate_EndQuery();
bool R_StreamUpdate_FindImageAndOptimize(const float *viewPos);
void R_StreamUpdatePerClient(const float *viewPos);
void R_StreamUpdate_End();
void R_Stream_UpdateStaticModelsCmd(void *data);
void R_Stream_UpdateStaticSurfacesCmd(void *data);
void R_Stream_SortCmd(void *data);
void R_Stream_CombineCmd(void *data);
int r_stream_update_staticmodelsCallback(jqBatch *batch);
int r_stream_update_staticsurfacesCallback(jqBatch *batch);
int r_stream_sortCallback(jqBatch *batch);
int r_stream_combineCallback(jqBatch *batch);
int r_stream_updateCallback(jqBatch *batch);

void Patch_R_Stream();