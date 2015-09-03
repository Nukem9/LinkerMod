#pragma once

typedef int Sys_ResetUpdateSpotLightEffectEvent_t(void);
static Sys_ResetUpdateSpotLightEffectEvent_t* Sys_ResetUpdateSpotLightEffectEvent = (Sys_ResetUpdateSpotLightEffectEvent_t*)0x0048D2D0;

typedef void Sys_ResetUpdateNonDependentEffectsEvent_t(void);
static Sys_ResetUpdateNonDependentEffectsEvent_t* Sys_ResetUpdateNonDependentEffectsEvent = (Sys_ResetUpdateNonDependentEffectsEvent_t*)0x0060F330;

typedef int __cdecl Sys_AddWorkerCmdInternal_t(void*, void*, void*);
static Sys_AddWorkerCmdInternal_t* Sys_AddWorkerCmdInternal = (Sys_AddWorkerCmdInternal_t*)0x005317A0;