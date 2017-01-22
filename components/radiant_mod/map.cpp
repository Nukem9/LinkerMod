#include "stdafx.h"

void(__thiscall * Map_LoadFile_o)(const char *filename);
void(*Map_SaveFile_o)(const char *filename, bool use_region, bool autosave);

void __fastcall hk_Map_LoadFile(const char *filename)
{
	// We're loading a new map - all old data is invalid
	Remote_EnableUpdates(false, true);
	Map_LoadFile_o(filename);
	Remote_EnableUpdates(true, true);
}

void hk_Map_SaveFile(const char *filename, bool useRegion, bool autosave)
{
	Remote_EnableUpdates(false);
	Map_SaveFile_o(filename, useRegion, autosave);
	Remote_EnableUpdates(true);
}
