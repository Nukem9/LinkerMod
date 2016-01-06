#pragma once
#include <windows.h>

#include "../sdk/public/steam/steam_api.h"
#pragma comment(lib, "../sdk/redistributable_bin/steam_api.lib")

#define APPID_BO1_SP	42700
#define APPID_BO1_TOOLS	42740

int AppInfo_Init(void);
const char* AppInfo_AppDir(void);
const char* AppInfo_IWDDir(void);
const char* AppInfo_RawDir(void);
