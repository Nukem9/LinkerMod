#pragma once

extern void(__thiscall * Map_LoadFile_o)(const char *filename);
extern void(*Map_SaveFile_o)(const char *filename, bool use_region, bool autosave);

void __fastcall hk_Map_LoadFile(const char *filename);
void hk_Map_SaveFile(const char *filename, bool useRegion, bool autosave);
