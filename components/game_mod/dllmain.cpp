#include "stdafx.h"
#include "patch_usefastfile.h"

bool g_initted = false;
BOOL GameMod_Init()
{
	ASSERT(g_initted == false);
	if(g_initted)
		return TRUE;

	//
	// Disable STDOUT buffering
	//
	setvbuf(stdout, nullptr, _IONBF, 0);

	//
	// Bypass CEG's code hashes
	//
	Patch_CEG();
	FixupFunction(0x0060CC10, 0x004F20F0);

	PatchUseFF();

	//
	// Sys_CheckCrashOrRerun, EAX = TRUE
	//
	PatchMemory(0x004F1930, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);

	//
	// Bypass deletion of 'steam_appid.txt'
	//
	PatchMemory(0x008B41AF, (PBYTE)"\x90\x90", 2);
	PatchMemory(0x008B41A5, (PBYTE)"\x90\x90\x90\x90\x90", 5); // fopen
	PatchMemory(0x008B41B3, (PBYTE)"\x90\x90\x90\x90\x90", 5); // fclose

	//
	// Force sv_vac 0
	//
	PatchMemory(0x005A272C, (PBYTE)"\x00", 1);
	PatchMemory(0x005A272E, (PBYTE)"\x00", 1);
	PatchMemory(0x00655051, (PBYTE)"\x90\x90\x90\x90\x90", 5);

	//
	// "com_introPlayed"
	// "com_startupIntroPlayed"
	//
	PatchMemory(0x0082C0F9, (PBYTE)"\x01", 1);
	PatchMemory(0x0082C111, (PBYTE)"\x01", 1);

	// TEMP CEG
	// 00580460
	PatchMemory(0x00580460, (PBYTE)"\xC3", 1);

	//
	// Always force the cursor to be shown
	//
	//PatchMemory(0x00683C50, (PBYTE)"\xC3", 1); // sub_683C50
	//PatchMemory(0x00867927, (PBYTE)"\xEB", 1); // jz short loc_86793E
	//PatchMemory(0x006EB2CC, (PBYTE)"\xEB", 1); // jz short loc_6EB2DE

	//
	// Command spam removal
	//
	PatchMemory(0x00887575, (PBYTE)"\x90\x90\x90\x90\x90", 5); // "Party_StopParty"
	PatchMemory(0x0043C6DB, (PBYTE)"\x90\x90\x90\x90\x90", 5); // "Clearing migration data\n"
	PatchMemory(0x0051B809, (PBYTE)"\x90\x90\x90\x90\x90", 5); // "Failed to log on.\n"

	//
	// EXE_TOOFEWPLAYERS in a party game
	//
	PatchMemory(0x00618D21, (PBYTE)"\x90\x90\x90\x90\x90", 5);

	//
	// PLATFORM_MISSINGMAP fix
	//
	SV_ValidateMap_o = Detours::X86::DetourFunction((PBYTE)0x0041EEC0, (PBYTE)&SV_ValidateMap);

	//
	// De-restrict Dvar_ForEachConsoleAccessName and
	// Cmd_ForEachConsoleAccessName
	//
	PatchMemory(0x005F5D4E, (PBYTE)"\x90\x90", 2);
	PatchMemory(0x00610A94, (PBYTE)"\x90\x90", 2);

	//
	// Enable the in-game console
	//
	Detours::X86::DetourFunction((PBYTE)0x00587DC8, (PBYTE)&Con_ToggleConsole);
	Detours::X86::DetourFunction((PBYTE)0x00587633, (PBYTE)&Con_ToggleConsole);
	PatchMemory(0x00587DC8, (PBYTE)"\xE8", 1);
	PatchMemory(0x00587633, (PBYTE)"\xE8", 1);
	PatchMemory(0x0058761C, (PBYTE)"\xEB", 1);

	//
	// Disable error message boxes with developer_script
	//
	PatchMemory(0x005A16F7, (PBYTE)"\x90\x90", 2);
	PatchMemory(0x005A1700, (PBYTE)"\x90\x90", 2);

	//
	// CL_Vid_Restart_Complete_f black screen fix
	//
	char *data = "vid_restart_complete; wait; map frontend;\n";
	PatchMemory(0x0045CD78, (PBYTE)&data, 4);

	//
	// Disable command restrictions
	//
	*(uint8_t **)&Cmd_ExecuteSingleCommandInternal = Detours::X86::DetourFunction((PBYTE)0x00829AD0, (PBYTE)&hk_Cmd_ExecuteSingleCommandInternal);

	//
	// Com_Init hook to actually enable the game's command line
	//
	*(uint8_t **)&Com_Init = Detours::X86::DetourFunction((PBYTE)0x004069C0, (PBYTE)&hk_Com_Init);

	//
	// CL_Vid_Restart_Complete_f hook to prevent crashes
	//
	*(uint8_t **)&CL_Vid_Restart_Complete_f = Detours::X86::DetourFunction((PBYTE)0x005D2F00, (PBYTE)&hk_CL_Vid_Restart_Complete_f); 

	//
	// UI_LoadModArenas hook to prevent leaked handles to mod.arena
	//
	Detours::X86::DetourFunction((PBYTE)0x0084D2A0, (PBYTE)&UI_LoadModArenas); 

	//
	// DB_ModXFileHandle hook to enable loading maps from mods
	//
	Detours::X86::DetourFunction((PBYTE)0x007A3610, (PBYTE)&DB_ModXFileHandle_hk);

	//
	// DB_LoadGraphicsAssetsForPC hook to automatically attempt to load frontend_patch.ff
	// which is can be used to enable the mods button on the main menu
	//
	Detours::X86::DetourFunction((PBYTE)0x00571DB0, (PBYTE)&DB_LoadGraphicsAssetsForPC);
	
	//
	// Enable loading of patches for custom maps
	//
	PatchMemory_WithNOP(0x00632350, 0x21);

	//
	// Enable mod verification when joining session or accepting invite
	//
	Session_Modify = (Session_Modify_t*)Detours::X86::DetourFunction((PBYTE)0x00611930, (PBYTE)&Session_Modify_Fix);
	Detours::X86::DetourFunction((PBYTE)0x00480CAC, (PBYTE)&Live_JoinSessionInProgressComplete_CheckMod);
	Detours::X86::DetourFunction((PBYTE)0x0046B314, (PBYTE)&Live_AcceptInviteAsyncComplete_CheckMod);
	PatchMemory_WithNOP(0x004521EE, 0x13);

	//
	// Nullify Sys_SetBlockSystemHotkeys (SetWindowsHookExA blocking)
	//
	PatchMemory(0x0040B397, (PBYTE)"\xEB", 1);
	PatchMemory(0x00868414, (PBYTE)"\xEB", 1);

	//
	// Force new mod specific configs to inherit from the vanilla config
	//
	Detours::X86::DetourFunction((PBYTE)0x0082A269, (PBYTE)&mfh_Cmd_Exec_f);

	//
	// Add Support for Mod Specific Bink Cinematics
	//
	Detours::X86::DetourFunction((PBYTE)0x006D98F0, (PBYTE)&hk_R_Cinematic_BinkOpen);

	//
	// Enable Custom Render Dvars
	//
	Detours::X86::DetourFunction((PBYTE)0x006CA27B, (PBYTE)&mfh_R_RegisterDvars);

	//
	// Misc Bug Fixes
	//
	Detours::X86::DetourFunction((PBYTE)0x007D9590, (PBYTE)&nullsub);
	
	//
	// Fix for misleading (incorrect) assertion message
	//
	const char* msg_assertion = "expected 'constant' or 'material', found '%s' instead\n";
	PatchMemory(0x00700492, (PBYTE)&msg_assertion, 4);

	//
	// Increase Asset Limits
	//
	DB_ReallocXAssetPool(ASSET_TYPE_WEAPON, 256);
	DB_ReallocXAssetPool(ASSET_TYPE_IMAGE, 6000);

	if(IsReflectionMode())
		ReflectionMod_Init();

	g_initted = true;
	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		return GameMod_Init(); 
	}
	else if(ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		if (ReflectionsWereUpdated() && IsInjectionMode())
			return InjectReflections();
	}

	return TRUE;
}
