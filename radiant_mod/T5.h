#pragma once

typedef void (__cdecl * Com_PrintError_t)(int channel, const char *fmt, ...);
static Com_PrintError_t Com_PrintError = (Com_PrintError_t)0x00468420;

typedef int (__cdecl * Com_sprintf_t)(char *dest, int size, const char *fmt, ...);
static Com_sprintf_t Com_sprintf = (Com_sprintf_t)0x004D3260;

typedef void *(__cdecl * Z_Malloc_t)(int size);
static Z_Malloc_t Z_Malloc = (Z_Malloc_t)0x004C7820;

typedef void (__cdecl * Z_Free_t)(void *ptr);
static Z_Free_t Z_Free = (Z_Free_t)0x004C77B0;