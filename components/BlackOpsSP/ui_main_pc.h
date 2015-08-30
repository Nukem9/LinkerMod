#pragma once

bool __cdecl UI_LoadModArenas();

typedef int (__cdecl* UI_ParseInfos_t)(const char *buf, int max, char **infos);
static UI_ParseInfos_t UI_ParseInfos = (UI_ParseInfos_t)0x005EF550;