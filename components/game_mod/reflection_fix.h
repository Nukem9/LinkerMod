#pragma once
#include <Windows.h>

BOOL IsReflectionMode();
BOOL ReflectionMod_Init();

void __cdecl R_GenerateReflectionRawData(DiskGfxReflectionProbe* probeRawData);