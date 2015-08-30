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

typedef void (__cdecl* DB_LoadXAssets_t)(XZoneInfo *zoneInfo, unsigned int zoneCount, int sync);
static DB_LoadXAssets_t DB_LoadXAssets = (DB_LoadXAssets_t)0x00631B10;

typedef bool (__cdecl* DB_ModFileExists_t)();
static DB_ModFileExists_t DB_ModFileExists = (DB_ModFileExists_t)0x005EEBF0;

typedef void (__cdecl* R_BeginRemoveScreenUpdate_t)();
static R_BeginRemoveScreenUpdate_t R_BeginRemoveScreenUpdate = (R_BeginRemoveScreenUpdate_t)0x006D7E60;

typedef int  (__cdecl* Sys_SyncDatabase_t)();
static Sys_SyncDatabase_t Sys_SyncDatabase = (Sys_SyncDatabase_t)0x0040DC40;

typedef void (__cdecl* R_EndRemoteScreenUpdate_t)(void (__cdecl *pumpfunc)());
static R_EndRemoteScreenUpdate_t R_EndRemoteScreenUpdate = (R_EndRemoteScreenUpdate_t)0x006D7EC0;

typedef void (__cdecl* SocketRouter_EmergencyFrame_t)(const char *from);
static SocketRouter_EmergencyFrame_t SocketRouter_EmergencyFrame = (SocketRouter_EmergencyFrame_t)0x004F11D0;

typedef void (__cdecl* DB_PostLoadXZone_t)();
static DB_PostLoadXZone_t DB_PostLoadXZone = (DB_PostLoadXZone_t)0x007A48D0;

void DB_SyncXAssets();
void DB_LoadGraphicsAssetsForPC();
void DB_ModXFileHandle_hk();
void DB_ModXFileHandle(HANDLE *zoneFile, char* zoneName, FF_DIR *zoneDir);