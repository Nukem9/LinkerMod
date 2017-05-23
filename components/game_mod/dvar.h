#pragma once

#define VANILLA_DVAR(NAME, ADDRESS) static dvar_s*& NAME = *(dvar_s **)ADDRESS;
#define VANILLA_VALUE(NAME, TYPE, ADDRESS) static TYPE& NAME = *(TYPE*)ADDRESS;

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

enum DvarSetSource
{
	DVAR_SOURCE_INTERNAL = 0x0,
	DVAR_SOURCE_EXTERNAL = 0x1,
	DVAR_SOURCE_SCRIPT = 0x2,
	DVAR_SOURCE_DEVGUI = 0x3,
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

VANILLA_DVAR(fs_gameDirVar, 0x025FADE8);
VANILLA_DVAR(fs_usermapDir, 0x025FADE4);
VANILLA_DVAR(fs_homepath, 0x025FBF0C);
VANILLA_DVAR(fs_basepath, 0x025FBF04);

VANILLA_DVAR(uiscript_debug, 0x02562A20);

VANILLA_DVAR(cl_ingame, 0x02910158);
VANILLA_DVAR(com_sv_running, 0x0243FD3C);
VANILLA_DVAR(sv_mapname, 0x02899CEC);

VANILLA_DVAR(developer, 0x0243FCA0);
VANILLA_DVAR(developer_script, 0x02481714);
VANILLA_DVAR(zombietron, 0x0247FDE8);
VANILLA_DVAR(zombiemode, 0x0243FDD4);
VANILLA_DVAR(blackopsmode, 0x0243FD24);
VANILLA_DVAR(useFastFile, 0x0247FEC8);
VANILLA_DVAR(g_connectpaths, 0x01C01850);

VANILLA_DVAR(r_reflectionProbeGenerate, 0x03B35038);
VANILLA_DVAR(r_reflectionProbeGenerateExit, 0x03B3503C);
VANILLA_DVAR(r_reflectionProbeRegenerateAll, 0x03B20030);

VANILLA_DVAR(ragdoll_reactivation_cutoff, 0xBCAEB0);
VANILLA_DVAR(ragdoll_explode_upbias, 0x251CBCC);
VANILLA_DVAR(ragdoll_explode_force, 0x251CBC8);

VANILLA_DVAR(g_debugBullets, 0x01BD9B48);
VANILLA_DVAR(sv_bullet_range, 0x02899D64);
VANILLA_DVAR(bg_forceExplosiveBullets, 0x00BCD1E8);
VANILLA_DVAR(vehicle_selfCollision, 0x00BCAFCC);
VANILLA_DVAR(player_sustainAmmo, 0x00BCD250);
VANILLA_DVAR(player_burstFireCooldown, 0x00BCD110);

VANILLA_DVAR(perk_weapRateMultiplier, 0x00BDF35C);
VANILLA_DVAR(perk_weapReloadMultiplier, 0x00BDF358);
VANILLA_DVAR(perk_weapSwitchMultiplier, 0x00BDF2C0);
extern dvar_s* perk_weapRateEnhanced;

VANILLA_DVAR(phys_gravity, 0x023D2FA8);

extern dvar_s* r_d3d9ex;
extern dvar_s* r_noborder;
extern dvar_s* r_showTess;
extern dvar_s* sm_quality;
extern dvar_s* con_extcon;
extern dvar_s* con_inputMaxMatchesShown;
extern dvar_s* cg_drawViewpos;
extern dvar_s* scr_useFastFileOnly;
extern dvar_s* scr_suppressErrors;
extern dvar_s* gm_build_date;
extern dvar_s* cg_showServerInfo;

VANILLA_DVAR(sys_configureGHz, 0x0276D9E0)
VANILLA_DVAR(sys_sysMB, 0x0276DAEC);
VANILLA_DVAR(sys_gpu, 0x0276D9E4);
VANILLA_DVAR(sys_configSum, 0x0276F558);

extern dvar_s* radiant_live;
extern dvar_s* radiant_livePort;
extern dvar_s* radiant_liveDebug;

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

typedef dvar_s *(__cdecl* Dvar_RegisterBool_t)(const char *dvarName, bool value, unsigned __int16 flags, const char *description);
static Dvar_RegisterBool_t Dvar_RegisterBool = (Dvar_RegisterBool_t)0x0045BB20;

typedef dvar_s *__cdecl Dvar_RegisterEnum_t(const char *dvarName, const char **valueList, int defaultIndex, unsigned int flags, const char *description);
static Dvar_RegisterEnum_t* Dvar_RegisterEnum = (Dvar_RegisterEnum_t*)0x0051BD00;


void mfh_R_RegisterDvars();

typedef void (__cdecl* CG_RegisterDvars_t)();
extern CG_RegisterDvars_t CG_RegisterDvars_o;

void __cdecl CG_RegisterDvars();

dvar_s *Dvar_SetFromStringByNameFromSource(const char *dvarName, const char *string, DvarSetSource source, unsigned int flags);
void Dvar_SetFromStringByName(const char *dvarName, const char *string);