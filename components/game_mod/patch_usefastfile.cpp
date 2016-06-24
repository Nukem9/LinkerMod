#include "patch_usefastfile.h"
#include <shellapi.h>

unsigned __int64 LiveSteam_GetUid()
{
	return 1337;
}

void PatchUseFF()
{
	//
	// Only apply patch if 'useFastFile 0' was present at launch
	//
	if (!LaunchArg_NoFF())
	{
		return;
	}

	//
	// Live_Init
	//
	PatchMemory_WithNOP(0x0046CBC6, 2);												// Live_InitPlatform => 'live_service' set to 0
	PatchMemory_WithNOP(0x0046CBD7, 2);												// ^

	PatchMemory_WithNOP(0x0046CCD8, 5);												// LiveGroups_Init
	PatchMemory_WithNOP(0x0046CD99, 5);												// Session_Init
	PatchMemory_WithNOP(0x0046CDA5, 5);												// LB_Init
	PatchMemory_WithNOP(0x0046CDAA, 5);												// LiveStorage_Init
	PatchMemory_WithNOP(0x0046CDAF, 5);												// Friends_Init
	PatchMemory_WithNOP(0x0046CDB9, 5);												// LiveCounter_Init
	PatchMemory_WithNOP(0x0082C949, 5);												// LiveSteam_Frame
	PatchMemory_WithNOP(0x005FEC99, 5);												// LiveSteam_Client_SteamDisconnect
	Detours::X86::DetourFunction((PBYTE)0x00501320, (PBYTE)LiveSteam_GetUid);		// LiveSteam_GetUid, nullptr crash fix

	BYTE data[] = "\xB8\x00\x00\x00\x00";
	PatchMemory(0x0057362A, data, 5);	// nullptr fix
	PatchMemory(0x00866C92, data, 5);	// ^

	//
	// Shaders
	//
	*(bool *)0x460C0EE = true;															// gIsPIMPEnabled
	*(bool *)0x460C108 = true;															// gCheckedForPimp
	Detours::X86::DetourFunction((PBYTE)0x007017B0, (PBYTE)hk_Material_CompileShader);	// Force cached shader loading from "shadercache_mods"

	//
	// Fastfile database
	//
	PatchMemory(0x006FBEC0, (PBYTE)"\xC3", 1);	// R_StreamAlloc_InitTempImages crash/horribly undefined behavior
	PatchMemory(0x006CBC3E, (PBYTE)"\x01", 1);  // Force 'r_streamFreezeState' set to 1 to avoid a crash

	PatchMemory_WithNOP(0x0050F8E7, 5);			// xGlobalsSingleton DB hang
	PatchMemory(0x0050F8F1, (PBYTE)"\xEB", 1);  // ^
	PatchMemory_WithNOP(0x0064FDDB, 5);			// GC_InitWeaponOptions DB hang
	PatchMemory_WithNOP(0x005DF328, 5);			// Sys_SyncDatabase DB hang

	//
	// Force multithreaded D3D device
	//
	PatchMemory_WithNOP(0x006B784D, 2);			// R_CreateDevice
}

bool LaunchArg_NoFF(void)
{
	int argc = 0;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);

	for (int i = 0; i < argc - 1; i++)
	{
		if (_wcsicmp(argv[i], L"useFastFile") == 0 && wcscmp(argv[i + 1], L"0") != 0)
		{
			return true;
		}
	}

	return false;
}
