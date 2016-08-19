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
	FixupFunction(0x0060CC10, 0x004F20F0);// CEGObfuscate<LiveStats_Init> => LiveStats_Init
	FixupFunction(0x00580460, 0x0079E6D0);// CEGObfuscate<Con_Restricted_SetLists> => Con_Restricted_SetLists

	//
	// Enable custom exception filter 
	//
	void* ptr = PrivateUnhandledExceptionFilter;
	PatchMemory(0x0050A7B0, (PBYTE)&ptr, 4);

	//
	// Add stack trace info to Sys_OutOfMemErrorInternal
	//
	Detours::X86::DetourFunction((PBYTE)0x004CFC30, (PBYTE)&Sys_OutOfMemErrorInternal, Detours::X86Option::USE_JUMP);

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
	// Unrestrict Dvar_ForEachConsoleAccessName,
	// Cmd_ForEachConsoleAccessName, and Dvar_ListSingle
	//
	PatchMemory_WithNOP(0x005F5D4E, 2);
	PatchMemory_WithNOP(0x00610A94, 2);
	PatchMemory_WithNOP(0x0082E63C, 6);

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
	char *data = "zombiemode 0; vid_restart_complete; wait; map frontend;\n";
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

#if _UNSTABLE
	//
	// Enable the use of level_dependencies.csv
	//
	PatchMemory_WithNOP(0x0082CA3B, 6);
	Detours::X86::DetourFunction((PBYTE)0x004C8890, (PBYTE)&Com_LoadLevelFastFiles);
#endif

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
	// Prevent joining a public (vanilla) lobby
	//
	Detours::X86::DetourFunction((PBYTE)0x008B3250, (PBYTE)&SV_UpdateCategory);

	//
	// Update protocol version (3141 -> 3142)
	//
	PatchMemory(0x00458EEA, (PBYTE)&g_protocolVersion, 4); // SV_DirectConnect
	PatchMemory(0x00458F2C, (PBYTE)&g_protocolVersion, 4); // SV_DirectConnect
	PatchMemory(0x004BA33E, (PBYTE)&g_protocolVersion, 4);
	PatchMemory(0x004DA935, (PBYTE)&g_protocolVersion, 4); // CL_ServerInfoPacket
	PatchMemory(0x0051E2D6, (PBYTE)&g_protocolVersion, 4);
	PatchMemory(0x0051E302, (PBYTE)&g_protocolVersion, 4);
	PatchMemory(0x0056F8D8, (PBYTE)&g_protocolVersion, 4); // SVC_Info
	PatchMemory(0x005705FE, (PBYTE)&g_protocolVersion, 4); // Session_StartHost
	PatchMemory(0x00573176, (PBYTE)&g_protocolVersion, 4);
	PatchMemory(0x005735FA, (PBYTE)&g_protocolVersion, 4); // dwLobbyEventHandler::onSessionStarted
	PatchMemory(0x00573601, (PBYTE)&g_protocolVersion, 4); // dwLobbyEventHandler::onSessionStarted
	PatchMemory(0x0059B20B, (PBYTE)&g_protocolVersion, 4); // Party_Awake
	PatchMemory(0x0059B27D, (PBYTE)&g_protocolVersion, 4); // Party_Awake
	PatchMemory(0x0059B29C, (PBYTE)&g_protocolVersion, 4); // Party_Awake

	PatchMemory(0x0060EAB9, (PBYTE)&g_protocolVersion, 4);
	PatchMemory(0x0065A2EE, (PBYTE)&g_protocolVersion, 4);
	PatchMemory(0x00675AE9, (PBYTE)&g_protocolVersion, 4);
	PatchMemory(0x006983CF, (PBYTE)&g_protocolVersion, 4); // SV_Init
	PatchMemory(0x006983D4, (PBYTE)&g_protocolVersion, 4); // SV_Init
	PatchMemory(0x006983D9, (PBYTE)&g_protocolVersion, 4); // SV_Init

	PatchMemory(0x0088CDD5, (PBYTE)&g_protocolVersion, 4); // JoinParty_f
	PatchMemory(0x0088CDDD, (PBYTE)&g_protocolVersion, 4); // JoinParty_f
	PatchMemory(0x008B333B, (PBYTE)&g_protocolVersion, 4);

	// 0x0057360E // A hook can be placed here to provide a notification when users attempt to invite you to a vanilla lobby

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
	// Add cg_showServerInfo dvar to show/hide server ip and name in coop scoreboards
	//
	CG_RegisterScoreboardDvars_o = (CG_RegisterScoreboardDvars_t)Detours::X86::DetourFunction((PBYTE)0x005C74D0, (PBYTE)&CG_RegisterScoreboardDvars);
	CL_GetServerIPAddress_o = (CL_GetServerIPAddress_t)Detours::X86::DetourFunction((PBYTE)0x0053BE60, (PBYTE)&CL_GetServerIPAddress);
	Detours::X86::DetourFunction((PBYTE)0x00890E23, (PBYTE)&mfh_CG_DrawBackdropServerInfo);

	//
	// Increase PMem size
	//
	unsigned int pmemSize = 0x12C00000 * 2;
	PatchMemory(0x00612302, (PBYTE)&pmemSize, 4);
	PatchMemory(0x00612341, (PBYTE)&pmemSize, 4);
	PatchMemory(0x0061234B, (PBYTE)&pmemSize, 4);

	//
	// Allow Path / Node Vis Print Calls to Print to Launcher's Console
	//
	PatchCall(0x004C4794, (PBYTE)Com_ToolPrintf); // Connecting paths...
	PatchCall(0x004C498D, (PBYTE)Com_ToolPrintf); // %d%%\n
	PatchCall(0x004C49BF, (PBYTE)Com_ToolPrintf); // Connecting paths done.
	
	PatchCall(0x004C47AD, (PBYTE)Com_ToolError); // Cannot calculate paths on a map_restart if paths already exist
	PatchCall(0x004C47CF, (PBYTE)Com_ToolError); // PATH_MAX_NODES (%i) exceeded.  Check log for nodecount

	PatchCall(0x0081705E, (PBYTE)Com_ToolPrintf); // Building node vis...
	PatchCall(0x008171B0, (PBYTE)Com_ToolPrintf); // %d%%\n
	PatchCall(0x008172DD, (PBYTE)Com_ToolPrintf); // %d%%\n
	PatchCall(0x00817303, (PBYTE)Com_ToolPrintf); // Building node vis done.

	//
	// Improve Clarity of Path / NodeVis Update Messages
	//
	const char* msg_pathUpdate = "Connecting paths: %d%%\n";
	PatchMemory(0x004C4983, (PBYTE)&msg_pathUpdate, 4);

	const char* msg_nodeVisUpdate = "visnode: %d%%\n";
	PatchMemory(0x008171A6, (PBYTE)&msg_nodeVisUpdate, 4);
	PatchMemory(0x008172D3, (PBYTE)&msg_nodeVisUpdate, 4);

	//
	// Increase Asset Limits
	//
	DB_ReallocXAssetPool(ASSET_TYPE_WEAPON, 256);

	int g_GfxImagePoolSize = LaunchArg_NoFF() ? 6000 : 4096;
	void* g_GfxImagePool_entries = (PBYTE)(DB_ReallocXAssetPool(ASSET_TYPE_IMAGE, g_GfxImagePoolSize)) + 4;
	PatchMemory(0x004561D8, (PBYTE)&g_GfxImagePool_entries, 4); // DB_GetImageAtIndex
	PatchMemory(0x005A1DC6, (PBYTE)&g_GfxImagePool_entries, 4); // DB_GetImageIndex

	PatchUseFF();

	//
	// Initialize either reflection mode or ReShade compatibility
	// depending on whether or not reflections are going to be calculated
	//
	IsReflectionMode() ? ReflectionMod_Init() : ReShade_Init();

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
		ReShade_Free();
		if (ReflectionsWereUpdated() && IsInjectionMode())
			return InjectReflections();
	}

	return TRUE;
}
