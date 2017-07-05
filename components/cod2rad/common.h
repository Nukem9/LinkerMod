#pragma once

// ...

typedef int(__cdecl* Com_Print_t)(const char* fmt, ...);
static Com_Print_t Com_Printf = (Com_Print_t)0x00441010;
static Com_Print_t Com_Error = (Com_Print_t)0x00440250;

typedef void(__cdecl* Com_AssembleImageFilepath_t)(const char* image, char* buf);
static Com_AssembleImageFilepath_t Com_AssembleImageFilepath = (Com_AssembleImageFilepath_t)0x0041E1A0;
