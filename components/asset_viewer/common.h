#pragma once

typedef void(__cdecl *Com_PrintError_t)(int channel, const char *fmt, ...);
static Com_PrintError_t Com_PrintError = (Com_PrintError_t)0x0040A0B0;

void Com_SuppressNoModelSpam(int channel, const char* fmt, const char* name);
