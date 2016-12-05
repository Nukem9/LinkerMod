#pragma once

#define MAX_MAP_REFLECTION_PROBES 64

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

struct GfxViewport
{
	int x;
	int y;
	int width;
	int height;
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

typedef int CubemapShot;

struct DiskGfxCubemapFace256
{
	char mip0[32768]; //256x256
	char mip1[8192]; //128x128
	char mip2[2048]; //64x64
	char mip3[512]; //32x32
	char mip4[128]; //16x16
	char mip5[32]; //8x8
	char mip6[8]; //4x4
	char mip7[8]; //2x2
	char mip8[8]; //1x1
};

struct DiskGfxReflectionProbeVolumeData
{
	float volumePlanes[6][4];
};

struct DiskGfxReflectionProbe
{
	float origin[3];

	union
	{
		char pixels[262224];
		DiskGfxCubemapFace256 face;
	};

	DiskGfxReflectionProbeVolumeData probeVolumes[16];
	unsigned int probeVolumeCount;
	char colorCorrectionFilename[64];
	char name[64];
};
static_assert(sizeof(DiskGfxReflectionProbe) == 0x406E0, "Size mismatch");

struct GfxReflectionProbeVolumeData
{
	float volumePlanes[6][4];
};

struct GfxReflectionProbe
{
	float origin[3];
	GfxImage *reflectionImage;
	GfxReflectionProbeVolumeData *probeVolumes;
	unsigned int probeVolumeCount;
};
static_assert(sizeof(GfxReflectionProbe) == 0x18, "Size mismatch");

void R_CalcCubeMapViewValues(refdef_s *refdef, CubemapShot cubemapShot, int cubemapSize);
void R_GenerateReflectionRawData(DiskGfxReflectionProbe* probeRawData);
void R_GenerateReflectionRawDataAll(DiskGfxReflectionProbe *probeRawData, int probeCount, bool *generateProbe);
bool R_CopyReflectionsFromLumpData(DiskGfxReflectionProbe *probeRawData, DiskGfxReflectionProbe *probeRawLumpData, const int lumpProbeCount);
void R_GenerateReflections(const char *mapname, GfxReflectionProbe *probes, const unsigned int probeCount);
bool R_ReflectionProbeGenerateExitWhenDone();
void R_GenerateReflectionImages(GfxReflectionProbe *probes, DiskGfxReflectionProbe *probeRawData, const int probeCount, const int probeBaseIndex);

void hk_R_GenerateReflections();