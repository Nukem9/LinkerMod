#pragma once
#include "r_debug.h"
#include "r_material.h"

struct GfxMatrix
{
	float m[4][4];
};

struct GfxCodeMatrices
{
	GfxMatrix matrix[32];
};

struct FxCodeMeshData
{
	char data[0x10];
};

struct GfxParticleCloud
{
	char data[0x40];
};

struct GfxSModelCachedVertex
{
	char data[0x20];
};

struct GfxModelLightingPatch
{
	char data[0x4C];
};

typedef int GfxBackEndPrimitiveData;

struct GfxEntity
{
	char data[0x18];
};

struct ShaderConstantSet
{
	char data[0x78];
};

struct GfxTextureOverride
{
	char data[0x10];
};


struct FxMarkMeshData
{
	char data[0x18];
};

struct GfxFog
{
	int startTime;
	int finishTime;
	float color[4];
	float fogStart;
	float density;
	float heightDensity;
	float baseHeight;
	float sunFogColor[4];
	float sunFogDir[3];
	float sunFogStartAng;
	float sunFogEndAng;
	float maxDensity;
};

struct GfxCmdHeader
{
	unsigned __int16 byteCount;
	char id;
	char ui3d;
};

struct GfxCmdArray
{
	char *cmds;
	int usedTotal;
	int usedCritical;
	GfxCmdHeader *lastCmd;
	int byteSize;
	int warnSize;
};

typedef int ShadowType;

struct PointLightPartition
{
	char data[0x1C0];
};

struct GfxMeshData
{
	char data[0x24];
};

struct GfxDrawSurfListInfo
{
	char data[0x50];
};

struct GfxExposureShaderRemap
{
	float remapMul[3];
	float remapAdd[3];
};

struct GfxViewRenderControl
{
	char mainSceneMSAA;
	char mainScene;
	char mainSceneFinal;
	char mainSceneSaved;
	char extraCam;
	char ui3d;
	char seeThruDecal;
	char frameBuffer;
	float sceneWindowU0;
	float sceneWindowV0;
	float sceneWindowU1;
	float sceneWindowV1;
	float framebufferWindowY0;
	float framebufferWindowX1;
	float framebufferWindowY1;
	int opaqueShaderRemap;
	int alphaShaderRemap;
	int emissiveShaderRemap;
	unsigned int renderingMode;
	unsigned int additionalPostFX;
};

struct GfxUI3DBackend
{
	char data[0xE0];
};

struct DynSModelGfxState
{
	char data[0x180C];
};


struct GfxFilm
{
	float filmMidStart;
	float filmMidEnd;
	float filmDarkFeather;
	float filmLightFeather;
	float filmBleach[3];
	float filmColorTemp[3];
	float filmHue[3];
	float filmSaturation[3];
	float filmDarkTint[3];
	float filmMidTint[3];
	float filmLightTint[3];
	float filmContrast[3];
	bool enabled;
	float filmLut;
	float sCurveEnable;
	float sCurveShoulderStrength;
	float sCurveLinearStrength;
	float sCurveLinearAngle;
	float sCurveToeStrength;
	float sCurveToeNumerator;
	float sCurveToeDenominator;
};

struct GfxBloom
{
	float bloomTintWeights[4];
	float bloomColorScale[4];
	float bloomTintScale[4];
	float bloomCurveBreakpoint[4];
	float bloomCurveLoBlack[4];
	float bloomCurveLoGamma[4];
	float bloomCurveLoWhite[4];
	float bloomCurveLoRemapBlack[4];
	float bloomCurveLoRemapWhite[4];
	float bloomCurveHiBlack[4];
	float bloomCurveHiGamma[4];
	float bloomCurveHiWhite[4];
	float bloomCurveHiRemapBlack[4];
	float bloomCurveHiRemapWhite[4];
	float bloomExpansionControl[4];
	float bloomExpansionWeights[4];
	int bloomExpansionSource;
	float bloomBlurRadius;
	float bloomPersistence;
	float bloomStreakXLevels0[4];
	float bloomStreakXLevels1[4];
	float bloomStreakXInnerTint[3];
	float bloomStreakXOuterTint[3];
	float bloomStreakXTintControl[4];
	float bloomStreakXTint[3];
	float bloomStreakYLevels0[4];
	float bloomStreakYLevels1[4];
	float bloomStreakYInnerTint[3];
	float bloomStreakYOuterTint[3];
	float bloomStreakYTintControl[4];
	float bloomStreakYTint[3];
};

struct GfxReviveFx
{
	bool enabled;
	float reviveEdgeColorTemp;
	float reviveEdgeSaturation;
	float reviveEdgeScale[3];
	float reviveEdgeContrast[3];
	float reviveEdgeOffset[3];
	float reviveEdgeMaskAdjust;
	float reviveEdgeAmount;
};

struct GfxLightScale
{
	float diffuseScale;
	float specularScale;
};

struct GfxVisionSet
{
	GfxFilm film;
	GfxBloom bloom;
	GfxReviveFx reviveFx;
	GfxLightScale charPrimaryLightScale;
};

struct GfxDepthOfField
{
	float viewModelStart;
	float viewModelEnd;
	float nearStart;
	float nearEnd;
	float farStart;
	float farEnd;
	float nearBlur;
	float farBlur;
};

struct GfxDoubleVision
{
	float direction[3];
	float motionBlurMagnitude;
	float deltaPerMS;
	float cur;
	float targ;
};

struct GfxGenericFilter
{
	bool passEnabled[3][16];
	void *passMaterial[3][16]; //Material*
	int passTarget[3][16];
	int passSampler0[3][16];
	int passSampler1[3][16];
	float passParam[3][16][16];
	int passQuads[3][16];
	float passFlareOcclusion[3][16];
	float sunPosition[3];
};

struct GfxPoison
{
	float curAmountTarget;
	float curAmount;
};

struct GfxCompositeFx
{
	float distortionScale[2];
	float blurRadius;
	float distortionMagnitude;
	float frameRate;
	int lastUpdate;
	int frame;
	int startMSec;
	int currentTime;
	int duration;
	bool enabled;
	bool scriptEnabled;
};

struct GfxSaveScreenParam
{
	float s0;
	float t0;
	float ds;
	float dt;
	int screenTimerId;
	char mode;
};

struct GfxBlendSaveScreenBlurredParam
{
	int fadeMsec;
	float s0;
	float t0;
	float ds;
	float dt;
	int screenTimerId;
	char enabled;
};

struct GfxBlendSaveScreenFlashedParam
{
	float intensityWhiteout;
	float intensityScreengrab;
	float s0;
	float t0;
	float ds;
	float dt;
	int screenTimerId;
	char enabled;
};

struct GfxSaveScreenFx
{
	GfxSaveScreenParam saveScreenParam;
	GfxBlendSaveScreenBlurredParam blendBlurredParam;
	GfxBlendSaveScreenFlashedParam blendFlashedParam;
};

struct float44
{
	union
	{
		float m[4][4];
		float member[16];
	};
};

struct GfxLightImage
{
	void *image;
	char samplerState;
};

struct GfxLightDef
{
	const char *name;
	GfxLightImage attenuation;
	int lmapLookupStart;
};

struct __declspec(align(16)) GfxLight
{
	char type;
	char canUseShadowMap;
	__int16 cullDist;
	float color[3];
	float dir[3];
	float origin[3];
	float radius;
	float cosHalfFovOuter;
	float cosHalfFovInner;
	int exponent;
	unsigned int spotShadowIndex;
	float angles[3];
	float spotShadowHiDistance;
	float diffuseColor[4];
	float specularColor[4];
	float shadowColor[4];
	float falloff[4];
	float attenuation[4];
	float aAbB[4];
	float cookieControl0[4];
	float cookieControl1[4];
	float cookieControl2[4];
	__declspec(align(8)) float44 viewMatrix;
	float44 projMatrix;
	GfxLightDef *def;
};

struct GfxExposureValue
{
	float blackPoint[4];
	float whitePoint[4];
	float linearStart[4];
	float linearEnd[4];
	float remapStart[4];
	float remapEnd[4];
	float scurveStart[4];
	float scurveEnd[4];
	float bloomCurveLDR[4];
	float bloomCurveHDR[4];
	float bloomScale[4];
};

struct WaterFogDef
{
	int startTime;
	int finishTime;
	float color[4];
	float fogStart;
	float density;
	float heightDensity;
	float baseHeight;
	float sunFogColor[4];
	float sunFogDir[3];
	float sunFogStartAng;
	float sunFogEndAng;
};

struct GfxSceneDef
{
	int time;
	float floatTime;
	float viewOffset[3];
};

struct GfxCodeImageRenderTargetFields
{
	__int64 _bf0;
};

union GfxCodeImageRenderTarget
{
	GfxCodeImageRenderTargetFields fields;
	unsigned int packed;
};

struct __declspec(align(16)) GfxCmdBufInput
{
	float consts[197][4];
	GfxImage *codeImages[43];
	char codeImageSamplerStates[43];
	GfxCodeImageRenderTarget codeImageRenderTargetControl[43];
	struct GfxBackEndData *data;
};

struct GfxSunShadow
{
	char data[0x450];
};

struct GfxSpotShadow
{
	char data[0x1D0];
};


struct GfxPlacement
{
	float quat[4];
	float origin[3];
};

struct GfxScaledPlacement
{
	GfxPlacement base;
	float scale;
};

enum GfxDepthRangeType
{
	GFX_DEPTH_RANGE_SCENE = 0x0,
	GFX_DEPTH_RANGE_VIEWMODEL = 0x2,
	GFX_DEPTH_RANGE_FULL = 0xFFFFFFFF,
};

enum GfxViewMode
{
	VIEW_MODE_NONE = 0x0,
	VIEW_MODE_3D = 0x1,
	VIEW_MODE_2D = 0x2,
	VIEW_MODE_IDENTITY = 0x3,
};

struct GfxViewport
{
	int x;
	int y;
	int width;
	int height;
};

enum GfxViewportBehavior
{
	GFX_USE_VIEWPORT_FOR_VIEW = 0x0,
	GFX_USE_VIEWPORT_FULL = 0x1,
};

struct GfxViewParms
{
	GfxMatrix viewMatrix;
	GfxMatrix projectionMatrix;
	GfxMatrix viewProjectionMatrix;
	GfxMatrix inverseViewProjectionMatrix;
	float origin[4];
	float axis[3][3];
	float depthHackNearClip;
	float zNear;
	float zFar;
};

struct __declspec(align(16)) GfxViewInfo
{
	char unk1[0x170];
	GfxSceneDef sceneDef;
	ShadowType dynamicShadowType;
	int localClientNum;
	int isRenderingFullScreen;
	int playerTeleported;
	bool needsFloatZ;
	bool renderingFloatZ;
	__declspec(align(16)) PointLightPartition pointLightPartitions[4];
	GfxMeshData pointLightMeshData[4];
	int pointLightCount;
	__declspec(align(16)) GfxLight visibleLights[4];
	int visibleLightCount;
	float blurRadius;
	float frustumPlanes[4][4];
	GfxLightScale charPrimaryLightScale;
	GfxDepthOfField dof;
	GfxFilm film;
	GfxCompositeFx flameFx;
	GfxReviveFx reviveFx;
	GfxCompositeFx waterSheetingFx;
	GfxDoubleVision doubleVision;
	GfxGenericFilter genericFilter;
	GfxPoison poisonFx;
	GfxCompositeFx electrifiedFx;
	GfxCompositeFx transportedFx;
	GfxSaveScreenFx saveScreenFx;
	const void *cmds;
	float sunVisibility;
	void *fullSceneViewMesh;
	GfxDrawSurfListInfo drawList[14];
	GfxCmdBufInput input;
	int renderSeeThruDecals;
	char hdrRenderingMode;
	GfxExposureValue exposureValue;
	GfxExposureShaderRemap exposureRemap;
	GfxViewRenderControl sceneComposition;
	GfxBloom bloom;
	float bloomExpansionXpos;
	float bloomExpansionYpos;
	GfxUI3DBackend rbUI3D;
	bool isMissileCamera;
	bool hasCmdBuf;
	float postEmissiveBrightening;
	bool noLodCullOut;
	void *dynSModelView;
	DynSModelGfxState *dynSModelState;
};

const struct __declspec(align(32)) GfxBackEndData
{
	char surfsBuffer[262144];
	FxCodeMeshData codeMeshes[2048];
	unsigned int primDrawSurfsBuf[65536];
	GfxViewParms viewParms[4];
	char primaryLightTechType[16][2][2][256];
	float codeMeshArgs[256][4];
	GfxParticleCloud clouds[256];
	GfxDrawSurf drawSurfs[32768];
	void *codeMeshPtr;
	__declspec(align(16)) GfxMeshData markMesh;
	__declspec(align(16)) GfxMeshData glassMesh;
	__declspec(align(16)) GfxSModelCachedVertex smcPatchVerts[8192];
	unsigned __int16 smcPatchList[256];
	unsigned int smcPatchCount;
	unsigned int smcPatchVertsUsed;
	GfxModelLightingPatch modelLightingPatchList[4096];
	volatile int modelLightingPatchCount;
	GfxBackEndPrimitiveData prim;
	unsigned int shadowableLightHasShadowMap[8];
	unsigned int frameCount;
	int drawSurfCount;
	volatile int surfPos;
	volatile int gfxEntCount;
	GfxEntity gfxEnts[256];
	volatile int shaderConstantSetCount;
	ShaderConstantSet shaderConstantSets[128];
	volatile int textureOverrideCount;
	GfxTextureOverride textureOverrides[256];
	volatile int cloudCount;
	volatile int codeMeshCount;
	volatile int codeMeshArgsCount;
	volatile int markMeshCount;
	FxMarkMeshData markMeshes[1536];
	volatile unsigned int *dynamicBufferCurrentFrame;
	void *skinnedCacheVb;
	void *endFence; //IDirect3DQuery9*
	char *tempSkinBuf;
	volatile int tempSkinPos;
	void *preTessIb;
	int viewParmCount;
	GfxFog fogSettings;
	GfxCmdArray *commands;
	unsigned int viewInfoIndex;
	unsigned int viewInfoCount;
	GfxViewInfo *viewInfo;
	GfxUI3DBackend rbUI3D;
	const void *cmds;
	const void *compositingCmds;
	__declspec(align(8)) GfxLight sunLight;
	int hasApproxSunDirChanged;
	volatile int primDrawSurfPos;
	DebugGlobals debugGlobals;
	unsigned int drawType;
	int hideMatureContent;
	int splitscreen;
	GfxLight shadowableLights[255];
	unsigned int shadowableLightCount;
	unsigned int emissiveSpotLightIndex;
	__declspec(align(16)) GfxLight emissiveSpotLight;
	int emissiveSpotDrawSurfCount;
	GfxDrawSurf *emissiveSpotDrawSurfs;
	unsigned int emissiveSpotLightCount;
	bool extraCamLargeFrameSize;
	GfxSunShadow sunShadow;
	unsigned int spotShadowCount;
	__declspec(align(16)) GfxSpotShadow spotShadows[4];
	DynSModelGfxState dynSModelState;
	void *dynSModelClientViewArray;
};

struct GfxPrimStats
{
	int primCount;
	int triCount;
	int staticIndexCount;
	int staticVertexCount;
	int dynamicIndexCount;
	int dynamicVertexCount;
};

struct GfxViewStats
{
	GfxPrimStats primStats[10];
	int drawSurfCount;
	int drawMatCount;
	int drawPrimHistogram[16];
};

struct GfxFrameStats
{
	GfxViewStats viewStats[2];
	int gfxEntCount;
	int geoIndexCount;
	int fxIndexCount;
};

struct GfxCmdBufPrimState
{
	union
	{
		IDirect3DDevice9 *device;
		IDirect3DDevice9 *localDevice;
	};

	IDirect3DIndexBuffer9 *indexBuffer;
	MaterialVertexDeclType vertDeclType;

	struct
	{
		unsigned int stride;
		IDirect3DVertexBuffer9 *vb;
		unsigned int offset;
	} streams[3];

	IDirect3DVertexDeclaration9 *vertexDecl;

	/*
	GfxFrameStats frameStats;
	struct GfxPrimStats *primStats;
	struct GfxPrimStats *backupPrimStats;
	struct GfxViewStats *viewStats;
	*/
};

struct GfxCmdBufSourceState
{
	GfxCodeMatrices matrices;
	GfxCmdBufInput input;
	GfxViewParms viewParms;
	GfxMatrix shadowLookupMatrix;
	unsigned __int16 constVersions[229];
	unsigned __int16 matrixVersions[8];
	float eyeOffset[4];
	unsigned int shadowableLightForShadowLookupMatrix;
	const GfxScaledPlacement *objectPlacement;
	const GfxViewParms *viewParms3D;
	unsigned int depthHackFlags;
	GfxScaledPlacement skinnedPlacement;
	int cameraView;
	GfxViewMode viewMode;
	GfxSceneDef sceneDef;
	GfxViewport sceneViewport;
	GfxViewport scissorViewport;
	float materialTime;
	float deibleBurnAmount;
	float deibleFadeAmount;
	float wetness;
	GfxViewportBehavior viewportBehavior;
	int renderTargetWidth;
	int renderTargetHeight;
	bool viewportIsDirty;
	bool scissorEnabled;
	unsigned int shadowableLightIndex;
};

STATIC_ASSERT_SIZE(GfxCmdBufSourceState, 0x1A90);

struct GfxCmdBufState
{
	char refSamplerState[16];
	unsigned int samplerState[16];
	GfxTexture *samplerTexture[16];
	GfxCmdBufPrimState prim;
	Material *material; // 0xC4
	char techType;
	MaterialTechnique *technique; // 0xCC
	MaterialPass *pass;
	unsigned int passIndex;
	GfxDepthRangeType depthRangeType;
	float depthRangeNear;
	float depthRangeFar;
	unsigned __int64 vertexShaderConstState[256];
	unsigned __int64 pixelShaderConstState[256];
	char alphaRef;
	unsigned int refStateBits[2];
	unsigned int activeStateBits[2];
	MaterialPixelShader *pixelShader;
	MaterialVertexShader *vertexShader;
	unsigned int pixPrimarySortKey;
	Material *pixMaterial;
	MaterialTechnique *pixTechnique;
	int pixCombine;
	GfxViewport viewport;
	GfxViewport scissor;
	int scissorEnabled;
	char renderTargetId;
	Material *origMaterial;
	char origTechType;
	int stateOverride;
};
STATIC_ASSERT_SIZE(GfxCmdBufState, 0x1148);

struct GfxCmdBufContext
{
	GfxCmdBufSourceState *source;
	GfxCmdBufState *state;
};

struct __declspec(align(16)) refdef_s
{
	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int height;
	float tanHalfFovX;
	float tanHalfFovY;
	float fov_x;
	float vieworg[3];
	float yaw;
	float viewaxis[3][3];
	int time;
	float zNear;
	float zFar;
	GfxVisionSet visionset;
	float blurRadius;
	GfxDepthOfField dof;
	GfxDoubleVision doubleVision;
	GfxCompositeFx flameFx;
	GfxCompositeFx waterSheetingFx;
	GfxGenericFilter genericFilter;
	GfxPoison poisonFx;
	GfxCompositeFx electrifiedFx;
	GfxCompositeFx transportedFx;
	GfxSaveScreenFx saveScreenFx;
	float sunVisibility;
	GfxLight primaryLights[255];
	GfxViewport scissorViewport;
	bool useScissorViewport;
	int localClientNum;
	int hideMatureContent;
	int splitscreen;
	int playerTeleported;
	int oldExposureId;
	int newExposureId;
	GfxExposureValue exposureValue;
	float lerpcount;
	int lastTime;
	unsigned int exposureMode;
	float exposure;
	float postEmissiveBrightening;
	bool noLodCullOut;
	WaterFogDef waterFog;
	float extraCamPos[3];
	bool extraCamPosValid;
	bool extraCamLargeFrame;
	float preExtraCamVieworg[3];
	float preExtraCamViewaxis[3][3];
	float preExtraCamTanHalfFovX;
	float preExtraCamTanHalfFovY;
};

typedef void __cdecl R_ClearScene_t(int localClientNum);
static R_ClearScene_t* R_ClearScene = (R_ClearScene_t*)0x006C7DC0;

typedef void __cdecl R_UpdateFrameFog_t(int localClientNum);
static R_UpdateFrameFog_t* R_UpdateFrameFog = (R_UpdateFrameFog_t*)0x006C0E90;

typedef GfxBackEndData* R_UpdateFrameSun_t(void);
static R_UpdateFrameSun_t* R_UpdateFrameSun = (R_UpdateFrameSun_t*)0x006C85D0;


typedef int __cdecl R_InitPrimaryLights_t(void*);
static R_InitPrimaryLights_t* R_InitPrimaryLights = (R_InitPrimaryLights_t*)0x006F6800;

typedef float __cdecl R_GetFarPlaneDist_t(void);
static R_GetFarPlaneDist_t* R_GetFarPlaneDist = (R_GetFarPlaneDist_t*)0x006B6350;
