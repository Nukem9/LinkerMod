#include "stdafx.h"

static_assert(GENERIC_READ == 0x80000000, "GENERIC_READ must have a value of 0x80000000");
static_assert((FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING) == 0x60000000, "(FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING) must have a value of 0x60000000");
static_assert(FILE_SHARE_READ == 0x1, "FILE_SHARE_READ must have a value of 0x1");
static_assert(OPEN_EXISTING == 0x3, "OPEN_EXISTING must have a value of 0x3");

// List of supported mod zone names
// These will be checked (in order) as long as a zone for the previous name exists
static const char* g_modZones[] = {
	"mod",
	"mod1",
	"mod2",
	"mod3",
	"mod4",
	"mod5",
	"mod6",
	"mod7"
};

XAssetEntry *DB_LinkXAssetEntry(XAssetEntry *newEntry, int allowOverride)
{
	// If yes, skip loading certain assets in this FF
	if (DB_IsLevelDependency(g_load_filename))
	{
		const char *assetName = DB_GetXAssetName(&newEntry->asset);
		XAssetType assetType = newEntry->asset.type;

		// Com_Printf(1, "Asset: [%01d] %s\n", assetType, assetName);

		switch (assetType)
		{
		case ASSET_TYPE_CLIPMAP:
		case ASSET_TYPE_CLIPMAP_PVS:
		case ASSET_TYPE_COMWORLD:
		case ASSET_TYPE_GAMEWORLD_SP:
		case ASSET_TYPE_GAMEWORLD_MP:
		case ASSET_TYPE_GFXWORLD:
		case ASSET_TYPE_GLASSES:
		case ASSET_TYPE_MATERIAL:
		case ASSET_TYPE_DESTRUCTIBLEDEF:
		case ASSET_TYPE_XMODEL:
		case ASSET_TYPE_WEAPON:
		case ASSET_TYPE_WEAPONDEF:
		case ASSET_TYPE_WEAPON_VARIANT:
			// Never load certain types (maps), regardless of what the user says
			return newEntry;

		// case ASSET_TYPE_SOUND:
		// case ASSET_TYPE_SOUND_PATCH:
		//	// NOTE: If excluded, SND_AddPatch/SND_AddBank must be fixed 
		//	break;

		default:
			// TODO: Allow certain asset types from the CSV. For now allow asset overrides.
			allowOverride = 1;
			break;
		}
	}

	return ((XAssetEntry *(__cdecl *)(XAssetEntry *, int))0x007A2F10)(newEntry, allowOverride);
}

const char *DB_GetXAssetName(XAsset *asset)
{
	ASSERT(asset);

	return DB_GetXAssetHeaderName(asset->type, &asset->header);
}

const char *DB_GetXAssetHeaderName(XAssetType type, XAssetHeader *header)
{
	ASSERT(header);
	ASSERT(DB_XAssetGetNameHandler[type]);
	ASSERT(header->data);

	const char *name = DB_XAssetGetNameHandler[type](header);
	ASSERT_MSG_VA(name, "Name not found for asset type %s\n", DB_GetXAssetTypeName(type));

	return name;
}

const char *DB_GetXAssetTypeName(int type)
{
	ASSERT(type >= 0 && type < ASSET_TYPE_COUNT);

	return g_assetNames[type];
}

void DB_SyncXAssets()
{
	R_BeginRemoteScreenUpdate();
	Sys_SyncDatabase();
	R_EndRemoteScreenUpdate(nullptr);
	SocketRouter_EmergencyFrame("DB_SyncXAssets");
	DB_PostLoadXZone();
}

void DB_LoadGraphicsAssetsForPC()
{
	unsigned int zoneCount = 1;
	
	XZoneInfo zoneInfo[5 + ARRAYSIZE(g_modZones)];
	zoneInfo[0].name = "code_post_gfx";
	zoneInfo[0].allocFlags = DB_ZONE_CODE;
	zoneInfo[0].freeFlags = DB_FLAG_STRICTFREE;

	DB_LoadXAssets(zoneInfo, zoneCount, 0);
	DB_SyncXAssets();

	zoneInfo[0].name = "patch";
	zoneInfo[0].allocFlags = DB_ZONE_PATCH;
	zoneInfo[0].freeFlags = DB_FLAG_NULL;

	//Add frontend_patch to the zone list (enable mods button)
	zoneInfo[zoneCount].name = "frontend_patch";
	zoneInfo[zoneCount].allocFlags = DB_ZONE_MOD;
	zoneInfo[zoneCount].freeFlags = DB_FLAG_NULL;
	zoneCount++;

	for (int modIndex = 0; modIndex < ARRAYSIZE(g_modZones); modIndex++)
	{
		// Check if the corresponding mod file exists
		// if it doesn't - we should stop looking for additional ones
		if (!DB_ModFileExists(modIndex))
			break;

		zoneInfo[zoneCount].name = g_modZones[modIndex];
		zoneInfo[zoneCount].allocFlags = DB_ZONE_MOD;
		zoneInfo[zoneCount].freeFlags = DB_FLAG_NULL;
		zoneCount++;	
	}
	

	DB_LoadXAssets(zoneInfo, zoneCount, 0);
}

void __declspec(naked) DB_ModXFileHandle_hk()
{
	__asm
	{
		push edx //zoneDir
		push esi //zoneName
		push edi //zoneFile
		call DB_ModXFileHandle
		add esp, 12
		retn
	}
}

void DB_ModXFileHandle(HANDLE *zoneFile, char* zoneName, FF_DIR *zoneDir)
{
	char filename[MAX_PATH];
	const char *gameModDir = (fs_gameDirVar) ? fs_gameDirVar->current.string : "";

	// Try loading /mods/fs_game/mod.ff
	if(strlen(gameModDir) > 0 && !strcmp(zoneName, "mod"))
	{
		sprintf_s(filename, "%s\\%s\\%s.ff", fs_homepath->current.string, gameModDir, zoneName);
		*zoneFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_OVERLAPPED, nullptr);

		if (*zoneFile != INVALID_HANDLE_VALUE)
		{
			*zoneDir = FFD_MOD_DIR;
			return;
		}
	}

	// Load custom maps associated with mods
	if(strlen(gameModDir) > 0 && strcmp(gameModDir, "raw") != 0)
	{
		sprintf_s(filename,"%s\\%s\\%s.ff", fs_homepath->current.string, gameModDir, zoneName);
		*zoneFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_OVERLAPPED, nullptr);

		if (*zoneFile != INVALID_HANDLE_VALUE)
		{
			*zoneDir = FFD_MOD_DIR;
			return;
		}
	}

	// Try /usermaps/map
	if (fs_usermapDir && fs_usermapDir->current.string)
	{
		const char* usermapDir = fs_usermapDir->current.string;
		sprintf_s(filename, "%s\\usermaps\\%s\\%s.ff", fs_homepath->current.string, usermapDir, zoneName);
		*zoneFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_OVERLAPPED, nullptr);

		if (*zoneFile != INVALID_HANDLE_VALUE)
		{
			*zoneDir = FFD_USER_MAP;
			return;
		}
	}
}

void* DB_ReallocXAssetPool(XAssetType type, unsigned int size)
{
	int assetSize = DB_GetXAssetTypeSize(type);
	void* assetPool = malloc(size * assetSize + sizeof(void*));
	DB_XAssetPool[type] = assetPool;
	g_poolSize[type] = size;
	
	return assetPool;
}

void __cdecl DB_ListAssetPool_f(void)
{
	if (Cmd_Argc() >= 2)
	{
		const char* val = Cmd_Argv(1);

		// Assume the user typed a number by default
		// Check if any non-numeric chars appear
		bool isNumeric = true;
		for (const char* c = val; *c; c++)
		{
			if (!isdigit(*c))
			{
				isNumeric = false;
				break;
			}
		}

		XAssetType type = ASSET_TYPE_COUNT;
		if (isNumeric)
		{
			type = (XAssetType)atoi(val);
			if (type < 0 || type >= ASSET_TYPE_COUNT)
				type = ASSET_TYPE_COUNT;
		}
		else
		{
			for (int i = 0; i < ASSET_TYPE_COUNT; i++)
			{
				if (strcmp(val, DB_GetXAssetTypeName(i)) == 0)
				{
					type = (XAssetType)i;
					break;
				}
			}
		}

		if (type != ASSET_TYPE_COUNT)
		{
			DB_ListAssetPool(type);
			return;
		}
	}

	Com_Printf(0, "listassetpool <poolnumber | poolname>: lists all the assets in the specified pool\n");
	for (int i = 0; i < ASSET_TYPE_COUNT; ++i)
	{
		const char* name = DB_GetXAssetTypeName(i);
		Com_Printf(0, "%d %s\n", i, name);
	}
}

bool DB_IsXAssetDefault(XAssetType type, const char *name)
{
	return ((bool (__cdecl *)(XAssetType, const char *))0x0062DDD0)(type, name);
}

void DB_ExternalInitAssets()
{
	BG_FillInAllWeaponItems();
}

unsigned int DB_GetImageIndex(GfxImage *image)
{
	//ASSERT(&entry->entry == image);
	//ASSERT(false, "index doesn't index ARRAY_COUNT(g_GfxImagePool.entries)");

	return ((char *)image - ((char *)DB_XAssetPool[ASSET_TYPE_IMAGE] + 4)) / sizeof(GfxImage);
}

GfxImage *DB_GetImageAtIndex(unsigned int index)
{
	// ASSERT(index < 0x1080, "index doesn't index ARRAY_COUNT(g_GfxImagePool.entries)");

	return (GfxImage *)(((char *)DB_XAssetPool[ASSET_TYPE_IMAGE] + 4) + (index * sizeof(GfxImage)));
	//return &g_GfxImagePool.entries[index].entry;
}

unsigned int DB_GetMaterialIndex(Material *material)
{
	//ASSERT(&entry->entry == material);
	//ASSERT(false, "index doesn't index ARRAY_COUNT(g_MaterialPool.entries)");

	return ((char *)material - ((char *)DB_XAssetPool[ASSET_TYPE_MATERIAL] + 8)) / sizeof(Material);
}

Material *DB_GetMaterialAtIndex(unsigned int index)
{
	// ASSERT(index < 0x1000, "index doesn't index ARRAY_COUNT(g_MaterialPool.entries)");

	return (Material *)(((char *)DB_XAssetPool[ASSET_TYPE_MATERIAL] + 8) + (index * sizeof(Material)));
	//return &g_MaterialPool.entries[index].entry;
}

unsigned int DB_GetXModelIndex(XModel *model)
{
	//ASSERT(&entry->entry == model);
	//ASSERT(false, "index doesn't index ARRAY_COUNT(g_XModelPool.entries)");

	return ((char *)model - ((char *)DB_XAssetPool[ASSET_TYPE_XMODEL] + 4)) / sizeof(XModel);
}

XModel *DB_GetXModelAtIndex(unsigned int index)
{
	// ASSERT(index < 0x3E8, "index doesn't index ARRAY_COUNT(g_XModelPool.entries)");

	return (XModel *)(((char *)DB_XAssetPool[ASSET_TYPE_XMODEL] + 4) + (index * sizeof(XModel)));
	//return &g_XModelPool.entries[index].entry;
}

int DB_FinishedLoadingAssets()
{
	return ((int(__cdecl *)())0x006714E0)();
}

GfxImage *DB_AllocTempImage()
{
	return ((GfxImage *(__cdecl *)())0x00528480)();
}

bool DB_ModFileExists(int index)
{
	ASSERT(index >= 0 && index < ARRAYSIZE(g_modZones));
	if (fs_gameDirVar == NULL || fs_homepath == NULL)
		return false;

	char filename[256];
	if (sprintf_s(filename, 256, "%s\\%s\\%s.ff", fs_homepath->current.string, fs_gameDirVar->current.string, g_modZones[index]) == -1)
		return false;

	HANDLE h = CreateFileA(filename, 0x80000000, 1u, 0, 3u, 0x60000000u, 0);
	if (h == INVALID_HANDLE_VALUE)
		return false;


	CloseHandle(h);
	return true;
}
