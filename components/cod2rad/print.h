#pragma once

typedef int(__cdecl* Com_FatalError_t)(char* format, ...);
static Com_FatalError_t Com_FatalError = (Com_FatalError_t)0x004294B0;

typedef void(__cdecl* BeginProgress_t)(const char* msg);
static BeginProgress_t BeginProgress = (BeginProgress_t)0x00429650;

typedef void(__cdecl* EndProgress_t)(void);
static EndProgress_t EndProgress = (EndProgress_t)0x004294C0;

typedef void(__cdecl* UpdateProgressPrint_t)();
static UpdateProgressPrint_t UpdateProgressPrint = (UpdateProgressPrint_t)0x00429500;

void __cdecl SetProgress(int current, int total);
void __cdecl UpdateProgress(int amount);
