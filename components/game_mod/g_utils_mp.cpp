#include "stdafx.h"

// /game_mp/g_utils_mp.cpp:202
int G_ModelIndex(const char *name)
{
	return ((int(__cdecl *)(const char *))0x004294B0)(name);
}

// /game_mp/g_utils_mp.cpp:842
void G_OverrideModel(int modelIndex, const char *defaultModelName)
{
	((void(__cdecl *)(int, const char *))0x0068C110)(modelIndex, defaultModelName);
}