#include "stdafx.h"

void** DB_XAssetPool = (void**)0x00B741B8;
DWORD* g_poolSize = (DWORD*)0x00B73EF8;

void DB_SyncXAssets()
{
	R_BeginRemoteScreenUpdate();
	Sys_SyncDatabase();
	R_EndRemoteScreenUpdate(0);
	SocketRouter_EmergencyFrame("DB_SyncXAssets");
	DB_PostLoadXZone();
}

void DB_LoadGraphicsAssetsForPC()
{
	unsigned int zoneCount = 1;
	
	XZoneInfo zoneInfo[6];
	zoneInfo[0].name = "code_post_gfx";
	zoneInfo[0].allocFlags = 1;
	zoneInfo[0].freeFlags = 0x80000000;

	DB_LoadXAssets(zoneInfo, zoneCount, 0);
	DB_SyncXAssets();

	zoneInfo[0].name = "patch";
	zoneInfo[0].allocFlags = 8;
	zoneInfo[0].freeFlags = 0;

	//Add frontend_patch to the zone list (enable mods button)
	zoneInfo[zoneCount].name = "frontend_patch";
	zoneInfo[zoneCount].allocFlags = 32;
	zoneInfo[zoneCount].freeFlags = 0;
	zoneCount++;

	if(DB_ModFileExists())
	{
		zoneInfo[zoneCount].name = "mod";
		zoneInfo[zoneCount].allocFlags = 32;
		zoneInfo[zoneCount].freeFlags = 0;
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
	char* str_mod = (char*)0x00A18DE8;

	HANDLE h = 0;
	char filename[MAX_PATH];
	
	if(fs_gameDirVar && fs_gameDirVar->current.string && !strcmp(zoneName,str_mod)) //Loads mods/fs_game/mod.ff
	{
		sprintf_s(filename, "%s\\%s\\%s.ff", fs_homepath->current.string, fs_gameDirVar->current.string, zoneName);
		HANDLE h = CreateFileA(filename, 0x80000000, 1u, 0, 3u, 0x60000000u, 0);
		*zoneFile = h;
		if ( h != (HANDLE)-1 )
			*zoneDir = FFD_MOD_DIR;
	}
	else if(strcmp(fs_gameDirVar->current.string, "raw") != 0 && strlen(fs_gameDirVar->current.string) > 0) //Load custom maps associated with mods
	{
		sprintf_s(filename,"%s\\%s\\%s.ff", fs_homepath->current.string, fs_gameDirVar->current.string, zoneName);
	
		h = CreateFileA(filename, 0x80000000, 1u, 0, 3u, 0x60000000u, 0);
		*zoneFile = h;
		if(h != (HANDLE)-1)
			*zoneDir = FFD_MOD_DIR;
	}
	else //Load usermaps/map
	{
		if(fs_usermapDir && fs_usermapDir->current.string)
		{
			const char* usermapDir = fs_usermapDir->current.string;
			sprintf_s(filename, "%s\\usermaps\\%s\\%s.ff", fs_homepath->current.string, usermapDir, zoneName);

			h = CreateFileA(filename, 0x80000000, 1u, 0, 3u, 0x60000000u, 0);
			*zoneFile = h;
			if(h != (HANDLE)-1)
				*zoneDir = FFD_USER_MAP;
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
