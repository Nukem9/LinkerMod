#include "stdafx.h"

BOOL GameMod_Init()
{
	//
	// Disable STDOUT buffering
	//
	setvbuf(stdout, nullptr, _IONBF, 0);

	//
	// Bypass CEG's code hashes
	//
	Patch_CEG();

	//
	// Enable custom exception filter 
	//
	void* ptr = PrivateUnhandledExceptionFilter;
	PatchMemory(0x0050A7B0, (PBYTE)&ptr, 4);

	//
	// Add stack trace info to Sys_OutOfMemErrorInternal
	//
	Detours::X86::DetourFunction((PBYTE)0x004CFC30, (PBYTE)&Sys_OutOfMemErrorInternal);

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
	// Disable steam kicking users for random reasons
	//
	PatchMemory(0x008B4240, (PBYTE)"\xC3", 1);// KickClientFromSteamGameServer
	PatchMemory(0x00616628, (PBYTE)"\xEB", 1);// Runframe

	//
	// "com_introPlayed"
	// "com_startupIntroPlayed"
	// "cg_fov_default"  (max 165.0)
	//
	PatchMemory(0x0082C0F9, (PBYTE)"\x01", 1);
	PatchMemory(0x0082C111, (PBYTE)"\x01", 1);
	PatchMemory(0x004A394A, (PBYTE)"\xD9\x05\xE4\x9F\xA1\x00", 6);
	PatchMemory(0x006CBE24, (PBYTE)"\x00", 1);
	
	DWORD flags = 0x1; // DVAR_ARCHIVED only
	PatchMemory(0x004A3921, (PBYTE)&flags, 4); // cg_fov
	PatchMemory(0x006CA4D6, (PBYTE)&flags, 4); // r_lodScaleRigid
	PatchMemory(0x006CA504, (PBYTE)&flags, 4); // r_lodBiasRigid
	PatchMemory(0x006CA53A, (PBYTE)&flags, 4); // r_lodScaleSkinned
	PatchMemory(0x006CA568, (PBYTE)&flags, 4); // r_lodBiasSkinned
	PatchMemory(0x006CBE29, (PBYTE)&flags, 4); // r_enablePlayerShadow

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
	PatchMemory(0x00659EDC, (PBYTE)"\x90\x90\x90\x90\x90", 5); // "Live_UpdateUiPopup: %s\n"

	//
	// EXE_TOOFEWPLAYERS in a party game
	//
	PatchMemory(0x00618D21, (PBYTE)"\x90\x90\x90\x90\x90", 5);

	//
	// PLATFORM_MISSINGMAP fix
	//
	SV_ValidateMap_o = Detours::X86::DetourFunction((PBYTE)0x0041EEC0, (PBYTE)&SV_ValidateMap);
	PartyClient_CheckMapExists_o = (PartyClient_CheckMapExists_t)Detours::X86::DetourFunction((PBYTE)0x0061B480, (PBYTE)&PartyClient_CheckMapExists);

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
	// Don't automatically close the console when loading a map
	//
	PatchMemory_WithNOP(0x0057011A, 5);
	PatchMemory_WithNOP(0x0051B0BD, 5);
	PatchMemory_WithNOP(0x00445AA1, 5);
	PatchMemory_WithNOP(0x00570130, 10);

	//
	// Disable error message boxes with developer_script
	//
	PatchMemory(0x005A16F7, (PBYTE)"\x90\x90", 2);
	PatchMemory(0x005A1700, (PBYTE)"\x90\x90", 2);

#if _DEBUG || _USE_COM_DPRINTF
	//
	// Enable Com_DPrintf
	//
	PatchMemory_WithNOP(0x0060F7E7, 2);

	PatchMemory_WithNOP(0x007A499B, 5); // "Redundant asset: '%s','%s'\n"
	PatchMemory_WithNOP(0x007A2E82, 5); // "Could not load %s \"%s\".\n"
	PatchMemory_WithNOP(0x007A2E9B, 5); // "Could not load %s \"%s\".\n"
	PatchMemory_WithNOP(0x007A2EC6, 5); // "Could not load %s \"%s\".\n"
	PatchMemory_WithNOP(0x004775E7, 5); // Scr_LoadScriptInternal
	PatchMemory_WithNOP(0x00477602, 5); // Scr_LoadScriptInternal
#endif

	//
	// Allow joining in-game (set party_joinInProgressAllowed)
	//
	PatchMemory_WithNOP(0x0088CFEA, 6);

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
	// Allow UI_FeederSelection_AllMaps to set ui_preview when an entry is highlighted
	// (Automatically set ui_preview to the highlighted map's loadName)
	//
	Detours::X86::DetourFunction((PBYTE)0x008352F6, (PBYTE)&mfh_UI_FeederSelection_AllMaps);

	//
	// DB_ModXFileHandle hook to enable loading maps from mods
	//
	Detours::X86::DetourFunction((PBYTE)0x007A3610, (PBYTE)&DB_ModXFileHandle_hk);

	//
	// Enable the use of level_dependencies.csv
	//
#if _UNSTABLE && _USE_LEVEL_DEPENDENCIES
	PatchMemory_WithNOP(0x0082CA3B, 6);
#endif

	//
	// Enable legacy mod (patch override) support
	// (This detour is used for both level_dependencies AND patch overrides)
	//
	Detours::X86::DetourFunction((PBYTE)0x004C8890, (PBYTE)&Com_LoadLevelFastFiles);

	//
	// Enable loading of common fastfiles in blackopsmode
	//
	Detours::X86::DetourFunction((PBYTE)0x0082CB50, (PBYTE)&Com_LoadCommonFastFile);

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
	// Enable Custom Dvars
	//
	Detours::X86::DetourFunction((PBYTE)0x006CA27B, (PBYTE)&mfh_R_RegisterDvars);
	CG_RegisterDvars_o = (CG_RegisterDvars_t)Detours::X86::DetourFunction((PBYTE)0x004A3860, (PBYTE)&CG_RegisterDvars);

	//
	// Add cg_showServerInfo dvar to show/hide server ip and name in coop scoreboards
	//
	CG_RegisterScoreboardDvars_o = (CG_RegisterScoreboardDvars_t)Detours::X86::DetourFunction((PBYTE)0x005C74D0, (PBYTE)&CG_RegisterScoreboardDvars);
	CL_GetServerIPAddress_o = (CL_GetServerIPAddress_t)Detours::X86::DetourFunction((PBYTE)0x0053BE60, (PBYTE)&CL_GetServerIPAddress);
	Detours::X86::DetourFunction((PBYTE)0x00890E23, (PBYTE)&mfh_CG_DrawBackdropServerInfo);

	//
	// Add Game_Mod Version info to the 'version' dvar
	//	can be viewed via 'cg_drawVersion 1'
	//
	static const char* version = "%s %s build %s %s\nGame_Mod - " __CONFIGURATION__ " - " __TIMESTAMP__;
	PatchMemory(0x0082D0D5, (PBYTE)&version, 4);

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
	// Live radiant initialization hook
	//
#if _UNSTABLE
	Detours::X86::DetourFunction((PBYTE)0x004B7870, (PBYTE)&RadiantRemoteInit);
#endif

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
	// Replace "Missing Image" assertion with silent warning (and add image to missingasset.csv)
	//
	PatchCall(0x0070A4CD, (PBYTE)&Image_HandleMissingImage);

	//
	// Utilize DirectX 9Ex when possible
	//
#if 0
	ptr = (void *)&hk_Direct3DCreate9;
	PatchMemory(0x009A34DC, (PBYTE)&ptr, 4);

	Detours::X86::DetourFunction((PBYTE)0x006B7597, (PBYTE)&hk_CreateDevice, Detours::X86Option::USE_CALL);
	Detours::X86::DetourFunction((PBYTE)0x0071F387, (PBYTE)&hk_GetSwapChain, Detours::X86Option::USE_CALL);

	Detours::X86::DetourFunction((PBYTE)0x0070A050, (PBYTE)&hk_Image_Create2DTexture_PC);
	Detours::X86::DetourFunction((PBYTE)0x0070A0F0, (PBYTE)&hk_Image_Create3DTexture_PC);
	Detours::X86::DetourFunction((PBYTE)0x0070A140, (PBYTE)&hk_Image_CreateCubeTexture_PC);

	PatchMemory(0x006EB4F1, (PBYTE)"\xEB", 1);
#endif

	//
	// Increase the maximum number of ragdolls
	//
	PatchMemory(0x00830076, (PBYTE)"\x40", 1); // Ragdoll_GetUnusedBody
	PatchMemory(0x00621694, (PBYTE)"\x40", 1); // ragdoll_max_simulating
	Detours::X86::DetourFunction((PBYTE)0x004672D0, (PBYTE)&Ragdoll_ExplosionEvent);

	//
	// Increase maximum number of reflection probes from 8
	//
	Detours::X86::DetourFunction((PBYTE)0x006CF200, (PBYTE)&hk_R_GenerateReflections);

	//
	// Increase Asset Limits
	//
	DB_ReallocXAssetPool(ASSET_TYPE_WEAPON, 256);
	DB_ReallocXAssetPool(ASSET_TYPE_XMODEL, 2048);

	void* g_MaterialPool_entries = (PBYTE)(DB_ReallocXAssetPool(ASSET_TYPE_MATERIAL, 4096)) + 8;
	PatchMemory(0x0052C28B, (PBYTE)&g_MaterialPool_entries, 4); // DB_GetMaterialAtIndex
	PatchMemory(0x005FB286, (PBYTE)&g_MaterialPool_entries, 4); // DB_GetMaterialIndex

	void* g_GfxImagePool_entries = (PBYTE)(DB_ReallocXAssetPool(ASSET_TYPE_IMAGE, 6000)) + 4;
	PatchMemory(0x004561D8, (PBYTE)&g_GfxImagePool_entries, 4); // DB_GetImageAtIndex
	PatchMemory(0x005A1DC6, (PBYTE)&g_GfxImagePool_entries, 4); // DB_GetImageIndex

	PatchUseFF();

	//
	// Initialize either reflection mode or ReShade compatibility
	// depending on whether or not reflections are going to be calculated
	//
	IsReflectionMode() ? ReflectionMod_Init() : ReShade_Init();

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
