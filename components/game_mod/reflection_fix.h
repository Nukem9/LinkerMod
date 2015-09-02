#pragma once
#include <Windows.h>

BOOL IsReflectionMode();
BOOL IsInjectionMode();
BOOL ReflectionMod_Init();

void hk_R_GenerateReflectionRawDataAll();
void __cdecl R_GenerateReflectionRawDataAll(DiskGfxReflectionProbe *probeRawData, int probeCount, bool *generateProbe);
void __cdecl R_GenerateReflectionRawData(DiskGfxReflectionProbe* probeRawData);

BOOL ReflectionsWereUpdated();
BOOL InjectReflections();