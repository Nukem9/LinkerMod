#pragma once

typedef int(*Com_GetBspChecksum_t)(void);
static Com_GetBspChecksum_t Com_GetBspChecksum = (Com_GetBspChecksum_t)0x00520F8F;

typedef void* (__cdecl* Com_LoadBsp_t)(char *filename);
static Com_LoadBsp_t Com_LoadBsp = (Com_LoadBsp_t)0x00520FDC;

typedef void (* Com_UnloadBsp_t)();
static Com_UnloadBsp_t Com_UnloadBsp = (Com_UnloadBsp_t)0x005210CC;

void mfh_Com_LoadBsp();
void __cdecl SV_SavePaths(char* buf, int size);