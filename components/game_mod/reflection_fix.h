#pragma once
#include <Windows.h>

BOOL IsReflectionMode();
BOOL IsInjectionMode();
BOOL ReflectionMod_Init();

void __cdecl R_GenerateReflectionRawData(DiskGfxReflectionProbe* probeRawData);

BOOL ReflectionsWereUpdated();
BOOL InjectReflections();