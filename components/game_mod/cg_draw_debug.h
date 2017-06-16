#pragma once

union float4
{
	float v[4];
	unsigned int u[4];
};

struct hybrid_vector
{
	float4 vec;
};
STATIC_ASSERT_SIZE(hybrid_vector, 16);

enum TraceHitType : int
{
	TRACE_HITTYPE_NONE = 0x0,
	TRACE_HITTYPE_ENTITY = 0x1,
	TRACE_HITTYPE_DYNENT_MODEL = 0x2,
	TRACE_HITTYPE_DYNENT_BRUSH = 0x3,
};
STATIC_ASSERT_SIZE(TraceHitType, 4);

struct trace_t
{
	hybrid_vector normal;
	float fraction;
	int sflags; // surfaceFlags
	int cflags; // contents
	TraceHitType hitType;
	unsigned __int16 hitId;
	unsigned __int16 modelIndex;
	unsigned __int16 partName;
	unsigned __int16 boneIndex;
	unsigned __int16 partGroup;
	bool allsolid;
	bool startsolid;
	bool walkable;
	struct cStaticModel_s *staticModel;
	int hitPartition;
};
STATIC_ASSERT_SIZE(trace_t, 0x38);

struct infoParm_t
{
	const char *name;
	int clearSolid;
	int surfaceFlags;
	int contents;
	int toolFlags;
};

class col_context_t
{
private:
	int mask;
	struct col_prim_t *prims;
	int nprims;
	struct IgnoreEntParams *ignoreEntParams;
	int passEntityNum0;
	int passEntityNum1;
	int staticmodels;
	int locational;
	char *priorityMap;
	int(__cdecl *collide_entity_func)(int, col_context_t *);

public:
	col_context_t(void);
	col_context_t(int _mask);
};

enum visionSetMode_t : int
{
	VISIONSETMODE_NAKED = 0x0,
	VISIONSETMODE_NIGHT = 0x1,
	VISIONSETMODE_FLARE = 0x2,
	VISIONSETMODE_INFRARED = 0x3,
	VISIONSETMODE_TVGUIDED = 0x4,
	VISIONSETMODE_EXTRACAM = 0x5,
	VISIONSETMODECOUNT = 0x6,
};


static infoParm_t* _infoParms = (infoParm_t*)0x00B7E938;

typedef int (__cdecl* CG_DrawDevString_t)(struct ScreenPlacement *scrPlace, float x, float y, float xScale, float yScale, const char *s, const float *color, int align, struct Font_s *font);
static CG_DrawDevString_t CG_DrawDevString = (CG_DrawDevString_t)0x00589F90;

typedef void (__cdecl* CG_DrawVersion_t)(void);
static CG_DrawVersion_t CG_DrawVersion = (CG_DrawVersion_t)0x0067DC00;

typedef void (__cdecl* CG_TraceCapsule_t)(trace_t *results, const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum, int contentMask, col_context_t *context);
static CG_TraceCapsule_t CG_TraceCapsule = (CG_TraceCapsule_t)0x0068FD90;

void CG_DrawMaterial(int localClientNum, int drawMaterialType);

void mfh_CG_DrawFullScreenDebugOverlays(void);
void __cdecl CG_DrawFullScreenDebugOverlays(int localClientNum);

struct visionSetVars_t
{
	bool filmEnable;
	float filmMidStart;
	float filmMidEnd;
	float filmDarkFeather;
	float filmLightFeather;
	float filmHue[3];
	float filmSaturation[3];
	float filmColorTemp[3];
	float filmLightTint[3];
	float filmMidTint[3];
	float filmDarkTint[3];
	float filmContrast[3];
	float filmBleach[3];
	float filmLut;
	bool reviveEnable;
	float reviveEdgeColorTemp;
	float reviveEdgeSaturation;
	float reviveEdgeScale[3];
	float reviveEdgeContrast[3];
	float reviveEdgeOffset[3];
	float reviveEdgeMaskAdjust;
	float reviveEdgeAmount;
	bool charPrimaryUseTweaks;
	float charPrimaryDiffuseScale;
	float charPrimarySpecularScale;
	float masterRingmod;
	float reverbRingmod;
	float hiFilter;
	float lowFilter;
	int sCurveEnable;
	float sCurveShoulderStrength;
	float sCurveLinearStrength;
	float sCurveLinearAngle;
	float sCurveToeStrength;
	float sCurveToeNumerator;
	float sCurveToeDenominator;
	float postEmissiveBrightening;
	float bloomBlurRadius;
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
	//char name[64];
};
STATIC_ASSERT_SIZE(visionSetVars_t, 672);

void mfh_CG_VisionSetApplyToRefdef(void);
