#pragma once
#include "vanilla.h"

#define DVAR_NOFLAG				0				// 0x0000
#define DVAR_ARCHIVE            (1 << 0)        // 0x0001
#define DVAR_USERINFO           (1 << 1)        // 0x0002
#define DVAR_SERVERINFO         (1 << 2)        // 0x0004
#define DVAR_SYSTEMINFO         (1 << 3)        // 0x0008
#define DVAR_INIT               (1 << 4)        // 0x0010
#define DVAR_LATCH              (1 << 5)        // 0x0020
#define DVAR_ROM                (1 << 6)        // 0x0040
#define DVAR_CHEAT              (1 << 7)        // 0x0080
#define DVAR_DEVELOPER          (1 << 8)        // 0x0100
#define DVAR_SAVED              (1 << 9)        // 0x0200
#define DVAR_NORESTART          (1 << 10)       // 0x0400
#define DVAR_CHANGEABLE_RESET   (1 << 12)       // 0x1000
#define DVAR_EXTERNAL           (1 << 14)       // 0x4000
#define DVAR_AUTOEXEC           (1 << 15)       // 0x8000
#define DVAR_UNRESTRICTED		(1 << 16)		// 0x10000

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

VANILLA_DVAR(cg_fov, 0x02FF6888);
VANILLA_DVAR(cg_fov_default, 0x02FF669C);
VANILLA_DVAR(cg_fov_default_thirdperson, 0x02F67B9C);
VANILLA_DVAR(cg_fovScale, 0x02FF66A8);

VANILLA_DVAR(g_changelevel_time, 0x01C018AC);
VANILLA_DVAR(g_reloading, 0x01C0186C);

VANILLA_DVAR(useSvMapPreloading, 0x02481784);
VANILLA_DVAR(useMapPreloading, 0x02481740);

VANILLA_DVAR(uiscript_debug, 0x02562A20);

VANILLA_DVAR(cl_demoLiveStreaming, 0x028D9DD0);

VANILLA_DVAR(cl_ingame, 0x02910158);
VANILLA_DVAR(com_sv_running, 0x0243FD3C);
VANILLA_DVAR(com_maxclients, 0x02481720);
VANILLA_DVAR(com_developer, 0x0243FCA0);
VANILLA_DVAR(sv_mapname, 0x02899CEC);
VANILLA_DVAR(sv_floodProtect, 0x02866FE4);

VANILLA_DVAR(developer_script, 0x02481714);
VANILLA_DVAR(zombietron, 0x0247FDE8);
VANILLA_DVAR(zombiemode, 0x0243FDD4);
VANILLA_DVAR(blackopsmode, 0x0243FD24);
VANILLA_DVAR(useFastFile, 0x0247FEC8);
VANILLA_DVAR(g_connectpaths, 0x01C01850);

VANILLA_DVAR(r_lockPvs, 0x03B1FD24);

VANILLA_DVAR(r_reflectionProbeGenerate, 0x03B35038);
VANILLA_DVAR(r_reflectionProbeGenerateExit, 0x03B3503C);
VANILLA_DVAR(r_reflectionProbeRegenerateAll, 0x03B20030);

VANILLA_DVAR(r_streamLog, 0x03B1FFC0);
VANILLA_DVAR(r_streamLowDetail, 0x03B1FCA4);
VANILLA_DVAR(r_streamClear, 0x03B1FDC0);
VANILLA_DVAR(r_stream, 0x03B1FB54);
VANILLA_DVAR(r_streamSize, 0x0396F9C4);
VANILLA_DVAR(r_streamFreezeState, 0x03B1FBCC);
VANILLA_DVAR(r_streamMaxDist, 0x0396F9CC);
VANILLA_DVAR(r_streamHiddenPush, 0x03B1FD18);
VANILLA_DVAR(r_loadForRenderer, 0x03B1FCC0);

VANILLA_DVAR(r_aspectRatio, 0x03B1FF88);
VANILLA_DVAR(com_wideScreen, 0x024817A8);

VANILLA_DVAR(cg_drawVersion, 0x02F67B14);
VANILLA_DVAR(cg_drawMaterial, 0x02FF676C);

VANILLA_DVAR(ragdoll_reactivation_cutoff, 0xBCAEB0);
VANILLA_DVAR(ragdoll_explode_upbias, 0x251CBCC);
VANILLA_DVAR(ragdoll_explode_force, 0x251CBC8);

VANILLA_DVAR(g_debugBullets, 0x01BD9B48);
VANILLA_DVAR(sv_bullet_range, 0x02899D64);
VANILLA_DVAR(bg_forceExplosiveBullets, 0x00BCD1E8);
VANILLA_DVAR(vehicle_selfCollision, 0x00BCAFCC);
VANILLA_DVAR(player_sustainAmmo, 0x00BCD250);
VANILLA_DVAR(player_burstFireCooldown, 0x00BCD110);
VANILLA_DVAR(player_clipSizeMultiplier, 0x00BCAF84);

VANILLA_DVAR(perk_weapRateMultiplier, 0x00BDF35C);
VANILLA_DVAR(perk_weapReloadMultiplier, 0x00BDF358);
VANILLA_DVAR(perk_weapSwitchMultiplier, 0x00BDF2C0);
extern dvar_s* perk_weapRateEnhanced;

VANILLA_DVAR(phys_gravity, 0x023D2FA8);

extern dvar_s* gm_build_date;
extern dvar_s* gm_version;

extern dvar_s* r_d3d9ex;
extern dvar_s* r_noborder;
extern dvar_s* r_showTess;
extern dvar_s* sm_quality;
extern dvar_s* con_extcon;
extern dvar_s* con_inputMaxMatchesShown;
extern dvar_s* cg_drawViewpos;
extern dvar_s* scr_useFastFileOnly;
extern dvar_s* scr_suppressErrors;
extern dvar_s* cg_showServerInfo;

VANILLA_DVAR(sys_configureGHz, 0x0276D9E0)
VANILLA_DVAR(sys_sysMB, 0x0276DAEC);
VANILLA_DVAR(sys_gpu, 0x0276D9E4);
VANILLA_DVAR(sys_configSum, 0x0276F558);

VANILLA_DVAR(showVisionSetDebugInfo, 0x00C23D40);

extern dvar_s* com_waitForStreamer;
extern dvar_s* com_developer_print;

extern dvar_s* sv_showCommands;

extern dvar_s* dw_cacheTicket;

extern dvar_s* radiant_live;
extern dvar_s* radiant_livePort;
extern dvar_s* radiant_liveDebug;

extern dvar_s* r_renderTweaks;
extern dvar_s* r_renderLit;
extern dvar_s* r_renderStandardPostFx;
extern dvar_s* r_renderDistortion;
extern dvar_s* r_renderEmissive;
extern dvar_s* r_renderCorona;
extern dvar_s* r_renderSuperflare;
extern dvar_s* r_renderSun;
extern dvar_s* r_renderReflected;
extern dvar_s* r_renderCloakHDR;
extern dvar_s* r_streamCheckAabb;

extern bool com_cfg_readOnly_default;
extern dvar_s* com_cfg_readOnly;

VANILLA_FUNC(Dvar_SetInAutoExec, void (__cdecl*)(bool inAutoExec), 0x0053D190);

typedef const char* Dvar_GetString_t(const char* dvarName);
static Dvar_GetString_t* Dvar_GetString = (Dvar_GetString_t*)0x0057FF80;

typedef void __cdecl Dvar_SetString_t(dvar_s *dvar, const char *value);
static Dvar_SetString_t* Dvar_SetString = (Dvar_SetString_t*)0x0044A2A0;

typedef void __cdecl Dvar_SetStringByName_t(const char *dvarName, const char *value);
static Dvar_SetStringByName_t* Dvar_SetStringByName = (Dvar_SetStringByName_t*)0x00666F30;

typedef void __cdecl Dvar_SetBool_t(dvar_s *dvar, bool value);
static Dvar_SetBool_t* Dvar_SetBool = (Dvar_SetBool_t*)0x004B0C10;

typedef void __cdecl Dvar_SetFloat_t(dvar_s *dvar, float value);
static Dvar_SetFloat_t* Dvar_SetFloat = (Dvar_SetFloat_t*)0x0061F7A9;

VANILLA_FUNC(Dvar_SetIntByName, void (__cdecl*)(const char *dvarName, int value), 0x00500100);

typedef dvar_s *__cdecl Dvar_RegisterString_t(const char *dvarName, const char *value, unsigned __int16 flags, const char *description);
static Dvar_RegisterString_t* Dvar_RegisterString = (Dvar_RegisterString_t*)0x0059B3B0;

typedef dvar_s *__cdecl Dvar_RegisterInt_t(const char *dvarName, int value, int min, int max, unsigned __int16 flags, const char *description);
static Dvar_RegisterInt_t* Dvar_RegisterInt = (Dvar_RegisterInt_t*)0x00651910;

typedef dvar_s *(__cdecl* Dvar_RegisterFloat_t)(const char *dvarName, float value, float min, float max, unsigned __int16 flags, const char *description);
static Dvar_RegisterFloat_t Dvar_RegisterFloat = (Dvar_RegisterFloat_t)0x00679020;

typedef dvar_s *(__cdecl* Dvar_RegisterBool_t)(const char *dvarName, bool value, unsigned __int16 flags, const char *description);
static Dvar_RegisterBool_t Dvar_RegisterBool = (Dvar_RegisterBool_t)0x0045BB20;

typedef dvar_s *__cdecl Dvar_RegisterEnum_t(const char *dvarName, const char **valueList, int defaultIndex, unsigned int flags, const char *description);
static Dvar_RegisterEnum_t* Dvar_RegisterEnum = (Dvar_RegisterEnum_t*)0x0051BD00;

VANILLA_FUNC(Dvar_FindVar, dvar_s *(__cdecl*)(const char *dvarName), 0x005AE810);

void mfh_R_RegisterDvars();

typedef void(__cdecl* CG_RegisterDvars_t)();
extern CG_RegisterDvars_t CG_RegisterDvars_o;

void __cdecl CG_RegisterDvars();

dvar_s *Dvar_SetFromStringByNameFromSource(const char *dvarName, const char *string, DvarSetSource source, unsigned int flags);
void Dvar_SetFromStringByName(const char *dvarName, const char *string);

void Dvar_ClearModified(dvar_s *dvar);

void Patch_DvarDescriptions();
