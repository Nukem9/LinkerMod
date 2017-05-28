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
