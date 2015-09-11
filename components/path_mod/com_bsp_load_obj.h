#pragma once

typedef void(__cdecl* Com_SaveLump_t)(int type, const void* newLump, unsigned int size, int behavior);
static Com_SaveLump_t Com_SaveLump = (Com_SaveLump_t)0x0052111F;

typedef int(*Com_GetBspChecksum_t)(void);
static Com_GetBspChecksum_t Com_GetBspChecksum = (Com_GetBspChecksum_t)0x00520F8F;

void mfh_Com_LoadBsp();
void __cdecl SV_SavePaths(char* buf, int size);