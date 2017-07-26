#pragma once

enum CriticalSection
{
	CRITSECT_FATAL_ERROR = 32,
	CRITSECT_STREAM_FORCE_LOAD_COMMAND = 45,
	CRITSECT_STREAM_SYNC_COMMAND = 46,
};

typedef void(__cdecl* Sys_EnterCriticalSection_t)(CriticalSection critSect);
static Sys_EnterCriticalSection_t Sys_EnterCriticalSection = (Sys_EnterCriticalSection_t)0x005FEA60;

typedef void(__cdecl* Sys_LeaveCriticalSection_t)(CriticalSection critSect);
static Sys_LeaveCriticalSection_t Sys_LeaveCriticalSection = (Sys_LeaveCriticalSection_t)0x0056D400;