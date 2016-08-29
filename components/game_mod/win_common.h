#pragma once

enum CriticalSection
{
	CRITSECT_FATAL_ERROR = 0x20
};

typedef void(__cdecl* Sys_EnterCriticalSection_t)(CriticalSection critSect);
static Sys_EnterCriticalSection_t Sys_EnterCriticalSection = (Sys_EnterCriticalSection_t)0x005FEA60;

void __cdecl Sys_OutOfMemErrorInternal(const char *filename, int line);
