#include "reshade.h"
#include "../shared/utility.h"
#include "patch_reflections.h"

HMODULE hRS = NULL;

BOOL ReShade_Init(void)
{
	if (IsReflectionMode())
	{
		return FALSE;
	}

	hRS = LoadLibraryA(RESHADE_DLL);

	if (hRS == NULL)
	{
		return FALSE;
	}

	void* pfn_Direct3DCreate9 = GetProcAddress(hRS, "Direct3DCreate9");
	void* pfn_D3DPERF_BeginEvent = GetProcAddress(hRS, "D3DPERF_BeginEvent");
	void* pfn_D3DPERF_EndEvent = GetProcAddress(hRS, "D3DPERF_EndEvent");

	if (pfn_Direct3DCreate9 == NULL || pfn_D3DPERF_BeginEvent == NULL || pfn_D3DPERF_EndEvent == NULL)
	{
		return FALSE;
	}

	void** pfnIA_Direct3DCreate9 = (void**)0x009A34DC;
	void** pfnIA_D3DPERF_BeginEvent = (void**)0x009A34E0;
	void** pfnIA_D3DPERF_EndEvent = (void**)0x009A34E4;

	Detours::X86::DetourFunction((PBYTE)*pfnIA_Direct3DCreate9, (PBYTE)pfn_Direct3DCreate9);
	Detours::X86::DetourFunction((PBYTE)*pfnIA_D3DPERF_BeginEvent, (PBYTE)pfn_D3DPERF_BeginEvent);
	Detours::X86::DetourFunction((PBYTE)*pfnIA_D3DPERF_EndEvent, (PBYTE)pfn_D3DPERF_EndEvent);

	return TRUE;
}

BOOL ReShade_Free(void)
{
	if (hRS == NULL)
		return TRUE;

	return FreeLibrary(hRS);
}
