#pragma once
#include <WTypes.h>

enum FF_DIR : DWORD
{
	FFD_DEFAULT,
	FFD_MOD_DIR,
	FFD_USER_MAP
};

void DB_ModXFileHandle_hk();
void DB_ModXFileHandle(HANDLE *zoneFile, char* zoneName, FF_DIR *zoneDir);