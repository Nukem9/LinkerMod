#pragma once

#include "r_reflection_probe.h"

struct FxCodeMeshData
{
	char data[0x10];
};

struct GfxMatrix
{
	float m[4][4];
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

struct GfxSceneDef
{
	char data[0x14];
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

struct GfxCodeImageRenderTarget
{
	char data[0x8];
};

struct __declspec(align(16)) GfxCmdBufInput
{
	float consts[197][4];
	void *codeImages[43];
	char codeImageSamplerStates[43];
	GfxCodeImageRenderTarget codeImageRenderTargetControl[43];
	void *data;
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

struct DebugGlobals
{
	char data[0x6C];
};

struct GfxDrawSurf
{
	char data[0x8];
};

struct GfxSunShadow
{
	char data[0x450];
};

struct GfxSpotShadow
{
	char data[0x1D0];
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