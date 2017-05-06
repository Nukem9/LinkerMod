#pragma once

struct __declspec(align(8)) SysInfo
{
	long double cpuGHz;
	long double configureGHz;
	int logicalCpuCount;
	int physicalCpuCount;
	int sysMB;
	char gpuDescription[512];
	bool SSE;
	char cpuVendor[13];
	char cpuName[49];
};

extern SysInfo& sys_info;

typedef int Sys_ResetUpdateSpotLightEffectEvent_t(void);
static Sys_ResetUpdateSpotLightEffectEvent_t* Sys_ResetUpdateSpotLightEffectEvent = (Sys_ResetUpdateSpotLightEffectEvent_t*)0x0048D2D0;

typedef void Sys_ResetUpdateNonDependentEffectsEvent_t(void);
static Sys_ResetUpdateNonDependentEffectsEvent_t* Sys_ResetUpdateNonDependentEffectsEvent = (Sys_ResetUpdateNonDependentEffectsEvent_t*)0x0060F330;

typedef int __cdecl Sys_AddWorkerCmdInternal_t(void*, void*, void*);
static Sys_AddWorkerCmdInternal_t* Sys_AddWorkerCmdInternal = (Sys_AddWorkerCmdInternal_t*)0x005317A0;

typedef const char* __cdecl Sys_Cwd_t(void);
static Sys_Cwd_t* Sys_Cwd = (Sys_Cwd_t*)0x0068B1A0;

typedef struct dvar_s* __cdecl Sys_RegisterInfoDvars_t();
static Sys_RegisterInfoDvars_t* Sys_RegisterInfoDvars = (Sys_RegisterInfoDvars_t*)0x00867980;

typedef void __cdecl Sys_ArchiveInfo_t(int checksum);
static Sys_ArchiveInfo_t* Sys_ArchiveInfo = (Sys_ArchiveInfo_t*)0x00610A00;

bool __cdecl Sys_HasInfoChanged(void);
