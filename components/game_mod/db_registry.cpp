#include "stdafx.h"

static_assert(GENERIC_READ == 0x80000000, "GENERIC_READ must have a value of 0x80000000");
static_assert((FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING) == 0x60000000, "(FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING) must have a value of 0x60000000");
static_assert(FILE_SHARE_READ == 0x1, "FILE_SHARE_READ must have a value of 0x1");
static_assert(OPEN_EXISTING == 0x3, "OPEN_EXISTING must have a value of 0x3");

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
	
	XZoneInfo zoneInfo[6];
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

	if(DB_ModFileExists())
	{
		zoneInfo[zoneCount].name = "mod";
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