#include "stdafx.h"

bool g_isPatched_WriteToConfig = false;
void Patch_WriteToConfig()
{
	if (g_isPatched_WriteToConfig)
		return;

	PatchCall(0x0082C525, (PBYTE)FS_FOpenFileWriteToDir); // Com_WriteConfigToFile
	PatchCall(0x0082C5A6, (PBYTE)FS_FOpenFileWriteToDir); // Com_WriteKeyConfigToFile

	g_isPatched_WriteToConfig = true;
}
