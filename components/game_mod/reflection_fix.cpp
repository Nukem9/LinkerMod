#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <shellapi.h>

char g_mapName[256];

BOOL IsReflectionMode()
{
	static bool hasChecked = false;
	static bool isReflectionMode = false;

	if(!hasChecked)
	{
		int argc = 0;
		LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);

		int flags = 0;
		for(int i = 0; i < argc-1; i++)
		{
			if(wcscmp(argv[i], L"+devmap") == 0 && flags ^ 1)
			{
				size_t convCount = 0;
				wcstombs_s(&convCount, g_mapName, argv[i+1],256);

				flags |= 1;
			}
			else if(wcscmp(argv[i], L"r_reflectionProbeGenerate") == 0 && wcscmp(argv[i+1], L"0") != 0 && flags ^ 2)
			{
				isReflectionMode = true;
				flags |= 2;
			}
			else if(flags == 3)
			{
				return isReflectionMode;
			}
		}
	}
	return isReflectionMode;
}

BOOL ReflectionMod_Init()
{
	if(!IsReflectionMode())
		return FALSE;

	//
	// Always force the cursor to be shown
	//
	//PatchMemory(0x00683C50, (PBYTE)"\xC3", 1); // sub_683C50
	//PatchMemory(0x00867927, (PBYTE)"\xEB", 1); // jz short loc_86793E
	//PatchMemory(0x006EB2CC, (PBYTE)"\xEB", 1); // jz short loc_6EB2DE

	//
	// Enable AutoExit Upon Completing Reflections
	//
	PatchMemory(0x006CF373, (PBYTE)"\x90\x90", 2);
	PatchMemory(0x006CF379, (PBYTE)"\x90\x90", 2);
	PatchMemory(0x006CF382, (PBYTE)"\x90\x90", 2);
	PatchMemory(0x006CF388, (PBYTE)"\x90\x90", 2);

	return TRUE;
}