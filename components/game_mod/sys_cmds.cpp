#include "stdafx.h"

SysInfo& sys_info = *(SysInfo*)0x0276F2F0;

bool __cdecl Sys_ShouldUpdateForInfoChange()
{
	Sys_ArchiveInfo(0);

	// Don't ask when building reflections or paths
	if (G_OnlyConnectingPaths() || r_reflectionProbeGenerate->current.integer != 0)
	{
		return false;
	}

	const char* title = Win_LocalizeRef("WIN_COMPUTER_CHANGE_TITLE");
	const char* body = Win_LocalizeRef("WIN_COMPUTER_CHANGE_BODY");
	const HWND hWnd = GetActiveWindow();

	return MessageBoxA(hWnd, body, title, MB_ICONINFORMATION | MB_YESNO) == IDYES;
}

bool __cdecl Sys_HasInfoChanged()
{
	Sys_RegisterInfoDvars();
	if (sys_configureGHz->current.value <= sys_info.configureGHz * 1.100000023841858
		&& sys_info.configureGHz * 0.8999999761581421 <= sys_configureGHz->current.value)
	{
		if (sys_sysMB->current.integer <= sys_info.sysMB + 32 && sys_sysMB->current.integer >= sys_info.sysMB - 32)
		{
			if (!strcmp(sys_gpu->current.string, sys_info.gpuDescription))
				return false;
		}
	}

	return Sys_ShouldUpdateForInfoChange();
}
