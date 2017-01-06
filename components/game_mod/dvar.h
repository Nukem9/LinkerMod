#pragma once

enum dvarType_t
{
	DVAR_TYPE_BOOL = 0x0,
	DVAR_TYPE_FLOAT = 0x1,
	DVAR_TYPE_FLOAT_2 = 0x2,
	DVAR_TYPE_FLOAT_3 = 0x3,
	DVAR_TYPE_FLOAT_4 = 0x4,
	DVAR_TYPE_INT = 0x5,
	DVAR_TYPE_ENUM = 0x6,
	DVAR_TYPE_STRING = 0x7,
	DVAR_TYPE_COLOR = 0x8,
	DVAR_TYPE_INT64 = 0x9,
	DVAR_TYPE_LINEAR_COLOR_RGB = 0xA,
	DVAR_TYPE_COLOR_XYZ = 0xB,
	DVAR_TYPE_COUNT = 0xC,
};

union DvarValue
{
	bool enabled;
	int integer;
	unsigned int unsignedInt;
	__int64 integer64;
	unsigned __int64 unsignedInt64;
	float value;
	float vector[4];
	const char *string;
	char color[4];
};

union DvarLimits
{
	struct
	{
		int stringCount;
		const char **strings;
	} enumeration;

	struct
	{
		int min;
		int max;
	} integer;

	struct
	{
		__int64 min;
		__int64 max;
	} integer64;

	struct
	{
		float min;
		float max;
	} value;

	struct
	{
		float min;
		float max;
	} vector;
};

struct dvar_s
{
	const char *name;
	const char *description;
	int hash;
	unsigned int flags;
	dvarType_t type;
	bool modified;
	bool loadedFromSaveGame;
	DvarValue current;
	DvarValue latched;
	DvarValue reset;
	DvarValue saved;
	DvarLimits domain;
	dvar_s *hashNext;
};
STATIC_ASSERT_OFFSET(dvar_s, current, 0x18);

static dvar_s*& fs_gameDirVar = *(dvar_s **)0x025FADE8;
static dvar_s*& fs_usermapDir = *(dvar_s **)0x025FADE4;
static dvar_s*& fs_homepath = *(dvar_s **)0x025FBF0C;
static dvar_s*& fs_basepath = *(dvar_s **)0x025FBF04;

static dvar_s*& com_sv_running = *(dvar_s **)0x0243FD3C;

static dvar_s*& developer = *(dvar_s **)0x0243FCA0;
static dvar_s*& developer_script = *(dvar_s **)0x02481714;
static dvar_s*& zombietron = *(dvar_s **)0x0247FDE8;
static dvar_s*& zombiemode = *(dvar_s **)0x0243FDD4;
static dvar_s*& blackopsmode = *(dvar_s **)0x0243FD24;
static dvar_s*& useFastFile = *(dvar_s **)0x0247FEC8;

static dvar_s*& r_reflectionProbeGenerate = *(dvar_s **)0x03B35038;
static dvar_s*& r_reflectionProbeGenerateExit = *(dvar_s **)0x03B3503C;
static dvar_s*& r_reflectionProbeRegenerateAll = *(dvar_s **)0x03B20030;

static dvar_s*& ragdoll_reactivation_cutoff = *(dvar_s **)0xBCAEB0;
static dvar_s*& ragdoll_explode_upbias = *(dvar_s **)0x251CBCC;
static dvar_s*& ragdoll_explode_force = *(dvar_s **)0x251CBC8;

static dvar_s*& g_debugBullets = *(dvar_s **)0x01BD9B48;
static dvar_s*& sv_bullet_range = *(dvar_s **)0x02899D64;
static dvar_s*& bg_forceExplosiveBullets = *(dvar_s **)0x00BCD1E8;
static dvar_s*& vehicle_selfCollision = *(dvar_s **)0x00BCAFCC;

extern dvar_s* r_d3d9ex;
extern dvar_s* r_noborder;
extern dvar_s* sm_quality;
extern dvar_s* con_extcon;
extern dvar_s* con_inputMaxMatchesShown;
extern dvar_s* cg_drawViewpos;
extern dvar_s* gm_build_date;
extern dvar_s* cg_showServerInfo;

typedef const char* Dvar_GetString_t(const char* dvarName);
static Dvar_GetString_t* Dvar_GetString = (Dvar_GetString_t*)0x0057FF80;

typedef void __cdecl Dvar_SetString_t(dvar_s *dvar, const char *value);
static Dvar_SetString_t* Dvar_SetString = (Dvar_SetString_t*)0x0044A2A0;

typedef void __cdecl Dvar_SetStringByName_t(const char *dvarName, const char *value);
static Dvar_SetStringByName_t* Dvar_SetStringByName = (Dvar_SetStringByName_t*)0x00666F30;

typedef void __cdecl Dvar_SetBool_t(dvar_s *dvar, bool value);
static Dvar_SetBool_t* Dvar_SetBool = (Dvar_SetBool_t*)0x004B0C10;

typedef dvar_s *__cdecl Dvar_RegisterString_t(const char *dvarName, const char *value, unsigned __int16 flags, const char *description);
static Dvar_RegisterString_t* Dvar_RegisterString = (Dvar_RegisterString_t*)0x0059B3B0;

typedef dvar_s *__cdecl Dvar_RegisterInt_t(const char *dvarName, int value, int min, int max, unsigned __int16 flags, const char *description);
static Dvar_RegisterInt_t* Dvar_RegisterInt = (Dvar_RegisterInt_t*)0x00651910;

void mfh_R_RegisterDvars();

typedef void (__cdecl* CG_RegisterDvars_t)();
extern CG_RegisterDvars_t CG_RegisterDvars_o;

void __cdecl CG_RegisterDvars();
