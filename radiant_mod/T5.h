#pragma once

enum errorParm_t
{
  ERR_FATAL = 0x0,
  ERR_DROP = 0x1,
  ERR_SERVERDISCONNECT = 0x2,
  ERR_DISCONNECT = 0x3,
  ERR_SCRIPT = 0x4,
  ERR_SCRIPT_DROP = 0x5,
  ERR_LOCALIZATION = 0x6,
};

typedef void (__cdecl * Com_PrintError_t)(int channel, const char *fmt, ...);
static Com_PrintError_t Com_PrintError = (Com_PrintError_t)0x00468420;

typedef int (__cdecl * Com_sprintf_t)(char *dest, int size, const char *fmt, ...);
static Com_sprintf_t Com_sprintf = (Com_sprintf_t)0x004D3260;

typedef void (__cdecl * Com_Error_t)(errorParm_t code, const char *fmt, ...);
static Com_Error_t Com_Error = (Com_Error_t)0x004B8480;

typedef void *(__cdecl * Z_Malloc_t)(int size);
static Z_Malloc_t Z_Malloc = (Z_Malloc_t)0x004C7820;

typedef void (__cdecl * Z_Free_t)(void *ptr);
static Z_Free_t Z_Free = (Z_Free_t)0x004C77B0;

typedef void (__cdecl * I_strncpyz_t)(char *dest, const char *src, int destsize);
static I_strncpyz_t I_strncpyz = (I_strncpyz_t)0x004D2E70;

typedef int (__cdecl * I_strnicmp_t)(const char *s0, const char *s1, int n);
static I_strnicmp_t I_strnicmp = (I_strnicmp_t)0x004D2EE0;

typedef void (__cdecl * Com_PrintWarning_t)(int channel, const char *fmt, ...);
static Com_PrintWarning_t Com_PrintWarning = (Com_PrintWarning_t)0x00468400;