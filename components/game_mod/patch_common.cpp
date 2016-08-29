#include "stdafx.h"

bool g_isPatched_WriteToConfig = false;
void Patch_Disable_WriteToConfig()
{
	if (g_isPatched_WriteToConfig)
		return;

	PatchMemory(0x0082C510, (PBYTE)"\xC3\x90\x90\x90\x90", 5);
	g_isPatched_WriteToConfig = true;
}
