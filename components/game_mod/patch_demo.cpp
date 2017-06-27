#include "stdafx.h"

void Patch_Demos()
{
	Detours::X86::DetourFunction((PBYTE)0x005A1AA0, (PBYTE)&CL_PlayDemo_f);
	Detours::X86::DetourFunction((PBYTE)0x004D6FD0, (PBYTE)&CL_Record_f);
}
