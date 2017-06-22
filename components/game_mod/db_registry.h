#pragma once
#include "bg_weapons_def.h"

enum FF_DIR : DWORD
{
	FFD_DEFAULT,
	FFD_MOD_DIR,
	FFD_USER_MAP
};

struct XZoneName
{
	char name[64];
	int flags;
	int fileSize;
	FF_DIR dir;
	bool loaded;
};
STATIC_ASSERT_SIZE(XZoneName, 0x50);

struct XZoneInfoInternal
{
	char name[64];
	int flags;
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

union XAssetHeader
{
	struct Material *material;
	struct MaterialPixelShader *pixelShader;
	struct MaterialVertexShader *vertexShader;
	struct MaterialTechniqueSet *techniqueSet;
	struct GfxImage *image;
	
	struct GfxLightDef *lightDef;
	
	WeaponVariantDef *weapon;

	void *data;
};

struct XAsset
{
	XAssetType type;
	XAssetHeader header;
};
STATIC_ASSERT_SIZE(XAsset, 0x8);

struct XAssetEntry
{
	XAsset asset;
	char zoneIndex;
	bool inuse;
	unsigned __int16 nextHash;
	unsigned __int16 nextOverride;
	unsigned __int16 usageFrame;
};

enum XZoneFlags
{
	DB_ZONE_NULL				= 0,
	DB_ZONE_CODE				= 1,		// 0x1,
	DB_ZONE_CODE_LOC			= 1 << 1,	// 0x2,

	DB_ZONE_DEV					= 1 << 2,	// 0x4, (IN SP: DB_ZONE_DEV == DB_ZONE_DEV_LOC )
	// DB_ZONE_DEV_LOC			= 1 << 2,	// 0x4,

	DB_ZONE_PATCH				= 1 << 3,	// 0x8,
	DB_ZONE_PATCH_LOC			= 1 << 4,	// 0x10,
	DB_ZONE_MOD					= 1 << 5,	// 0x20,
	DB_ZONE_MOD_LOC				= 1 << 6,	// 0x40,
	DB_ZONE_COMMON				= 1 << 8,	// 0x100,
	DB_ZONE_COMMON_LOC			= 1 << 8,	// 0x100,
	
#if 0
	// These values need to be validated
	DB_ZONE_FFOTD				= 1 << 9,	// 0x200,
	DB_ZONE_FFOTD_LOC			= 1 << 10,	// 0x400,
	DB_ZONE_LEVELCOMMON			= 1 << 11,	// 0x800,
	DB_ZONE_LEVELCOMMON_LOC		= 1 << 12,	// 0x1000,
	DB_ZONE_LEVEL				= 1 << 13,	// 0x2000,
	DB_ZONE_LEVEL_LOC			= 1 << 14,	// 0x4000,
	DB_ZONE_LEVELOVERLAY		= 1 << 15,	// 0x8000,
	DB_ZONE_LEVELOVERLAY_LOC	= 1 << 16,	// 0x10000,
#endif

	DB_ZONE_GUMP				= 1 << 17,	// 0x20000,
	DB_ZONE_GUMP_LOC			= 1 << 18,	// 0x40000,

#if 0
	// These values need to be validated
	DB_ZONE_LOW_MARKER			= 1 << 19,	// 0x80000,
	DB_ZONE_MYCHANGES_LOC		= 1 << 20,	// 0x100000,
	DB_ZONE_MYCHANGES			= 1 << 21,	// 0x200000,

	// Probably not used in SP
	DB_ZONE_UI_VIEWER_LOC		= 1 << 22,	// 0x400000,
	DB_ZONE_UI_VIEWER			= 1 << 23,	// 0x800000,

	DB_ZONE_FRONTEND_LOC		= 1 << 24,	// 0x1000000,
	DB_ZONE_FRONTEND			= 1 << 25,	// 0x2000000,
#endif

	DB_ZONE_PATCH_UI			= 1 << 26, // 0x4000000,
	DB_ZONE_PATCH_UI_LOC		= 1 << 27, // 0x8000000,

#if 0
	// These values need to be validated
	DB_ZONE_HIGH_MARKER			= 1 << 26,	// 0x4000000,
	DB_ZONE_LOAD				= 1 << 27,	// 0x8000000,
#endif

	DB_FLAG_NULL				= 0,
	DB_FLAG_RSTREAM				= 1 << 30,	// 0x40000000,
	DB_FLAG_STRICTFREE			= 1 << 31,	// 0x80000000,
};

VANILLA_VALUE(g_zoneInfoCount, volatile unsigned int, 0x00E72984);
static XZoneInfoInternal *g_zoneInfo = (XZoneInfoInternal *)0x00C84308;
static XZoneName *g_zoneNames = (XZoneName *)0x010C6608;
static void **DB_XAssetPool = (void **)0x00B741B8;
static DWORD *g_poolSize = (DWORD *)0x00B73EF8;
static char **g_assetNames = (char **)0x00B73BB0;
VANILLA_VALUE(g_load_filename, char*, 0x00C7934C);

static auto DB_XAssetGetNameHandler = (const char *(__cdecl **)(XAssetHeader *))0x00B73C60;

typedef void (__cdecl* DB_LoadXAssets_t)(XZoneInfo *zoneInfo, unsigned int zoneCount, int sync);
static DB_LoadXAssets_t DB_LoadXAssets = (DB_LoadXAssets_t)0x00631B10;

typedef int (__cdecl* DB_ReleaseXAssets_t)();
static DB_ReleaseXAssets_t DB_ReleaseXAssets = (DB_ReleaseXAssets_t)0x0062C260;

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

typedef void(__cdecl* DB_LogMissingAsset_t)(XAssetType type, const char *name);
static DB_LogMissingAsset_t DB_LogMissingAsset = (DB_LogMissingAsset_t)0x004AEC20;

typedef void(__cdecl* DB_EnumXAssets_t)(XAssetType type, void(__cdecl *func)(XAssetHeader, void *), void *inData, bool includeOverride);
static DB_EnumXAssets_t DB_EnumXAssets = (DB_EnumXAssets_t)0x0054C1C0;

typedef void (__cdecl* DB_ListAssetPool_t)(XAssetType type);
static DB_ListAssetPool_t DB_ListAssetPool = (DB_ListAssetPool_t)0x007A2C00;

XAssetEntry *DB_LinkXAssetEntry(XAssetEntry *newEntry, int allowOverride);
const char *DB_GetXAssetName(XAsset *asset);
const char *DB_GetXAssetHeaderName(XAssetType type, XAssetHeader *header);
const char *DB_GetXAssetTypeName(int type);
void DB_SyncXAssets();
void DB_LoadGraphicsAssetsForPC();
void DB_ModXFileHandle_hk();
void DB_ModXFileHandle(HANDLE *zoneFile, char* zoneName, FF_DIR *zoneDir);
void* DB_ReallocXAssetPool(XAssetType type, unsigned int size);
void __cdecl DB_ListAssetPool_f(void);
bool DB_IsXAssetDefault(XAssetType type, const char *name);
void DB_ExternalInitAssets();