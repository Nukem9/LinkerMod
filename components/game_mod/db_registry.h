#pragma once
#include <WTypes.h>

enum FF_DIR : DWORD
{
	FFD_DEFAULT,
	FFD_MOD_DIR,
	FFD_USER_MAP
};

struct XZoneInfo
{
	const char *name;
	int allocFlags;
	int freeFlags;
};

enum XAssetType
{
	ASSET_TYPE_XMODELPIECES,
	ASSET_TYPE_PHYSPRESET,
	ASSET_TYPE_PHYSCONSTRAINTS,
	ASSET_TYPE_DESTRUCTIBLEDEF,
	ASSET_TYPE_XANIMPARTS,
	ASSET_TYPE_XMODEL,
	ASSET_TYPE_MATERIAL,
	ASSET_TYPE_TECHNIQUE_SET,
	ASSET_TYPE_IMAGE,
	ASSET_TYPE_SOUND,
	ASSET_TYPE_SOUND_PATCH,
	ASSET_TYPE_CLIPMAP,
	ASSET_TYPE_CLIPMAP_PVS,
	ASSET_TYPE_COMWORLD,
	ASSET_TYPE_GAMEWORLD_SP,
	ASSET_TYPE_GAMEWORLD_MP,
	ASSET_TYPE_MAP_ENTS,
	ASSET_TYPE_GFXWORLD,
	ASSET_TYPE_LIGHT_DEF,
	ASSET_TYPE_UI_MAP,
	ASSET_TYPE_FONT,
	ASSET_TYPE_MENULIST,
	ASSET_TYPE_MENU,
	ASSET_TYPE_LOCALIZE_ENTRY,
	ASSET_TYPE_WEAPON,
	ASSET_TYPE_WEAPONDEF,
	ASSET_TYPE_WEAPON_VARIANT,
	ASSET_TYPE_SNDDRIVER_GLOBALS,
	ASSET_TYPE_FX,
	ASSET_TYPE_IMPACT_FX,
	ASSET_TYPE_AITYPE,
	ASSET_TYPE_MPTYPE,
	ASSET_TYPE_MPBODY,
	ASSET_TYPE_MPHEAD,
	ASSET_TYPE_CHARACTER,
	ASSET_TYPE_XMODELALIAS,
	ASSET_TYPE_RAWFILE,
	ASSET_TYPE_STRINGTABLE,
	ASSET_TYPE_PACK_INDEX,
	ASSET_TYPE_XGLOBALS,
	ASSET_TYPE_DDL,
	ASSET_TYPE_GLASSES,
	ASSET_TYPE_EMBLEMSET,
	ASSET_TYPE_COUNT
};


extern void** DB_XAssetPool;
extern DWORD* g_poolSize;

typedef void (__cdecl* DB_LoadXAssets_t)(XZoneInfo *zoneInfo, unsigned int zoneCount, int sync);
static DB_LoadXAssets_t DB_LoadXAssets = (DB_LoadXAssets_t)0x00631B10;

typedef bool (__cdecl* DB_ModFileExists_t)();
static DB_ModFileExists_t DB_ModFileExists = (DB_ModFileExists_t)0x005EEBF0;

typedef void (__cdecl* DB_ResetZoneSize_t)(int trackLoadProgress);
static DB_ResetZoneSize_t DB_ResetZoneSize = (DB_ResetZoneSize_t)0x00621530;

typedef int(__cdecl* Sys_SyncDatabase_t)();
static Sys_SyncDatabase_t Sys_SyncDatabase = (Sys_SyncDatabase_t)0x0040DC40;

typedef void (__cdecl* R_BeginRemoteScreenUpdate_t)();
static R_BeginRemoteScreenUpdate_t R_BeginRemoteScreenUpdate = (R_BeginRemoteScreenUpdate_t)0x006D7E60;

typedef void (__cdecl* R_EndRemoteScreenUpdate_t)(void (__cdecl *pumpfunc)());
static R_EndRemoteScreenUpdate_t R_EndRemoteScreenUpdate = (R_EndRemoteScreenUpdate_t)0x006D7EC0;

typedef void (__cdecl* SocketRouter_EmergencyFrame_t)(const char *from);
static SocketRouter_EmergencyFrame_t SocketRouter_EmergencyFrame = (SocketRouter_EmergencyFrame_t)0x004F11D0;

typedef void (__cdecl* DB_PostLoadXZone_t)();
static DB_PostLoadXZone_t DB_PostLoadXZone = (DB_PostLoadXZone_t)0x007A48D0;

typedef int (__cdecl* DB_GetXAssetTypeSize_t)(int type);
static DB_GetXAssetTypeSize_t DB_GetXAssetTypeSize = (DB_GetXAssetTypeSize_t)0x00514840;

void DB_SyncXAssets();
void DB_LoadGraphicsAssetsForPC();
void DB_ModXFileHandle_hk();
void DB_ModXFileHandle(HANDLE *zoneFile, char* zoneName, FF_DIR *zoneDir);

void* DB_ReallocXAssetPool(XAssetType type, unsigned int size);
