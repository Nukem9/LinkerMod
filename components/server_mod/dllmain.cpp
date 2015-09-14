#include "stdafx.h"

char* dwInitAddress = " http://cod7-steam-auth.live.demonware.net";
char *dwLSGAddress = "";

typedef int(__cdecl * DB_GetXAssetSizeHandler_t)();
DB_GetXAssetSizeHandler_t* DB_GetXAssetSizeHandlers = (DB_GetXAssetSizeHandler_t*)0x00E064A0;

void** DB_XAssetPool = (void**)0x00E06888;
unsigned int* g_poolSize = (unsigned int*)0x00E065C8;

void* ReallocateAssetPool(int type, unsigned int newSize)
{
	int elSize = DB_GetXAssetSizeHandlers[type]();
	void* poolEntry = malloc(newSize * elSize);
	DB_XAssetPool[type] = poolEntry;
	g_poolSize[type] = newSize;
	return poolEntry;
}

struct netadr_t
{
	int	type;		 // this+0x0
	unsigned char	ip[4];		 // this+0x4
	unsigned short	port;		 // this+0x8
	int	addrHandleIndex;		 // this+0xC
};

bool (* dwCommonAddrToNetadr)(netadr_t *adr, const char *commonAddrBuf, void *secID);

// 006490E0
bool hk_dwCommonAddrToNetadr(netadr_t *adr, const char *commonAddrBuf, void *secID)
{
	bool ret = dwCommonAddrToNetadr(adr, commonAddrBuf, secID);

	if ((DWORD)_ReturnAddress() == 0x00676D8A)
	{
		adr->type = 2;
		adr->ip[0] = 192;
		adr->ip[1] = 168;
		adr->ip[2] = 1;
		adr->ip[3] = 200;
		adr->port = 3124;
	}

	return ret;
}

enum XAssetType
{
	ASSET_TYPE_XMODELPIECES = 0x0,
	ASSET_TYPE_PHYSPRESET = 0x1,
	ASSET_TYPE_PHYSCONSTRAINTS = 0x2,
	ASSET_TYPE_DESTRUCTIBLEDEF = 0x3,
	ASSET_TYPE_XANIMPARTS = 0x4,
	ASSET_TYPE_XMODEL = 0x5,
	ASSET_TYPE_MATERIAL = 0x6,
	ASSET_TYPE_TECHNIQUE_SET = 0x7,
	ASSET_TYPE_IMAGE = 0x8,
	ASSET_TYPE_SOUND = 0x9,
	ASSET_TYPE_SOUND_PATCH = 0xA,
	ASSET_TYPE_CLIPMAP = 0xB,
	ASSET_TYPE_CLIPMAP_PVS = 0xC,
	ASSET_TYPE_COMWORLD = 0xD,
	ASSET_TYPE_GAMEWORLD_SP = 0xE,
	ASSET_TYPE_GAMEWORLD_MP = 0xF,
	ASSET_TYPE_MAP_ENTS = 0x10,
	ASSET_TYPE_GFXWORLD = 0x11,
	ASSET_TYPE_LIGHT_DEF = 0x12,
	ASSET_TYPE_UI_MAP = 0x13,
	ASSET_TYPE_FONT = 0x14,
	ASSET_TYPE_MENULIST = 0x15,
	ASSET_TYPE_MENU = 0x16,
	ASSET_TYPE_LOCALIZE_ENTRY = 0x17,
	ASSET_TYPE_WEAPON = 0x18,
	ASSET_TYPE_WEAPONDEF = 0x19,
	ASSET_TYPE_WEAPON_VARIANT = 0x1A,
	ASSET_TYPE_SNDDRIVER_GLOBALS = 0x1B,
	ASSET_TYPE_FX = 0x1C,
	ASSET_TYPE_IMPACT_FX = 0x1D,
	ASSET_TYPE_AITYPE = 0x1E,
	ASSET_TYPE_MPTYPE = 0x1F,
	ASSET_TYPE_MPBODY = 0x20,
	ASSET_TYPE_MPHEAD = 0x21,
	ASSET_TYPE_CHARACTER = 0x22,
	ASSET_TYPE_XMODELALIAS = 0x23,
	ASSET_TYPE_RAWFILE = 0x24,
	ASSET_TYPE_STRINGTABLE = 0x25,
	ASSET_TYPE_PACK_INDEX = 0x26,
	ASSET_TYPE_XGLOBALS = 0x27,
	ASSET_TYPE_DDL = 0x28,
	ASSET_TYPE_GLASSES = 0x29,
	ASSET_TYPE_EMBLEMSET = 0x2A,
	ASSET_TYPE_COUNT = 0x2B,
	ASSET_TYPE_STRING = 0x2B,
	ASSET_TYPE_ASSETLIST = 0x2C,
};

bool Live_IsSignedIn(int controllerIndex)
{
	return true;
}

void DevGui_AddCommand(const char *path, const char *command)
{
}

void(*SteamAPI_Init)();

void(*LiveSteam_Init)();
void hk_LiveSteam_Init()
{
	HMODULE hSteam = GetModuleHandleA("steam_api.dll");

	*(FARPROC *)&SteamAPI_Init = GetProcAddress(hSteam, "SteamAPI_Init");

	SteamAPI_Init();
	LiveSteam_Init();
}

BOOL ServerMod_Init()
{
	PatchMemory(0x0059E83E, (PBYTE)&dwInitAddress, 4);
	PatchMemory(0x0059EE78, (PBYTE)&dwInitAddress, 4);

	//PatchMemory(0x0059EE78, (PBYTE)&dwLSGAddress, 4);

	ReallocateAssetPool(ASSET_TYPE_STRINGTABLE, 1024);
	ReallocateAssetPool(ASSET_TYPE_LOCALIZE_ENTRY, 20400);
	ReallocateAssetPool(ASSET_TYPE_SNDDRIVER_GLOBALS, 16);

	PatchMemory(0x005A0274, (PBYTE)"\x90\x90\x90\x90\x90", 5);
	PatchMemory(0x0071023F, (PBYTE)"\x90\x90\x90\x90\x90", 5);
	PatchMemory(0x0093CA00, (PBYTE)"\xC3", 1);

	PatchMemory(0x008BD8EB, (PBYTE)"\x90\x90\x90\x90\x90", 5);
	PatchMemory(0x008BD7E5, (PBYTE)"\x90\x90\x90\x90\x90", 5);

	//
	// Com_Init_Try_Block_Function
	//
	{
		// CL_InitDedicated
		PatchMemory_WithNOP(0x006D49BF, 7);

		// Client initializations
		PatchMemory_WithNOP(0x006D4D2A, 2);

		// Renderer
		PatchMemory_WithNOP(0x006D4D96, 2);
	}

	// !C .P !D
	// Com_Frame_Try_Block_Function
	//
	{
		// Renderer
		PatchMemory_WithNOP(0x006D6C6D, 6);

		// todo: 006D6979
		// todo: 006D69F3

		// This needs to be rewritten to handle DW login
		// DWDedicatedLobbyPump
		//PatchMemory(0x006D6E27, (PBYTE)"\xEB", 1);
	}

	// !C .P !D
	// Com_Init
	//
	{
		// Renderer
		PatchMemory_WithNOP(0x006D424D, 2);

		// todo: check if this should be enabled or not
		//PatchMemory(0x006D42F1, (PBYTE)"\xEB", 1);
	}

	// !C .P !D
	// Com_Frame
	//
	{
		// Renderer
		PatchMemory_WithNOP(0x006D68CE, 2);
	}

	// !C .P !D
	// SV_SpawnServer
	//
	{
		// Client setup
		PatchMemory_WithNOP(0x00710666, 2);

		// Exit after tool completion
		PatchMemory_WithNOP(0x00710C4A, 6);
	}

	//
	// RELATED FF LOADING
	//
	{
		// !C .P !D Com_LoadUiFastFile
		PatchMemory_WithNOP(0x006D5DCD, 7);

		// !C .P !D Com_InitUIAndCommonXAssets
		PatchMemory_WithNOP(0x006D410B, 9);

		// !C .P !D DB_LoadFastFilesForPC
		PatchMemory_WithNOP(0x00594EF4, 2);
	}

	// todo: 006D4342 Com_ErrorCleanup
	// todo: 006F587A Sys_WaitRenderer

	// !C .P !D UI_SetLoadingScreenMaterial
	PatchMemory_WithNOP(0x0077697B, 7);

	// !C .P !D PC_InitSigninState
	PatchMemory(0x009911A2, (PBYTE)"\xEB", 1);

	// !C .P !D Com_LoadMapLoadingScreenFastFile
	PatchMemory_WithNOP(0x006D5F0B, 4);

	// SND_ShouldInit short-circuit fix
	PatchMemory(0x0093110B, (PBYTE)"\x90\x90\x90\x90\x90\x90", 6);

	// Com_IntroPlayed
	PatchMemory(0x006D500A, (PBYTE)"\x90\x90\x90\x90", 4);

	// Com_WriteConfigToFile
	PatchMemory(0x006D5A8C, (PBYTE)"\x90\x90", 2);

	// Com_WriteKeyConfigToFile
	PatchMemory(0x006D5BEF, (PBYTE)"\x90\x90", 2);

	// !C !P !D Com_LoadFrontEnd
	PatchMemory_WithNOP(0x006D66BA, 9);

	// TODO: CHECK: 007077CB

	PatchMemory(0x006D410B, (PBYTE)"\x90\x90", 2);

	// script
	PatchMemory(0x008DC190, (PBYTE)"\xEB", 1);
	PatchMemory(0x00661FD0, (PBYTE)"\xC3", 1);// scr_playerconnect

	// Live_GetXuid
	PatchMemory(0x00990994, (PBYTE)"\xEB", 1);

	// xuid != PCACHE_INVALID_XUID assert
	PatchMemory(0x0096FD07, (PBYTE)"\xEB", 1);
	PatchMemory(0x0096F8AC, (PBYTE)"\xEB", 1);
	PatchMemory(0x0096FA6B, (PBYTE)"\xEB", 1);

	// Duplicate QPORT on server
	PatchMemory(0x007075E0, (PBYTE)"\xEB", 1);

	// VAC: MOV EDX, 0
	PatchMemory(0x0097F72B, (PBYTE)"\xBA\x00\x00\x00\x00", 5);

	Detours::X86::DetourFunction((PBYTE)0x009908C0, (PBYTE)&Live_IsSignedIn);
	Detours::X86::DetourFunction((PBYTE)0x00596EB0, (PBYTE)&DevGui_AddCommand);

	*(uint8_t **)&LiveSteam_Init = Detours::X86::DetourFunction((PBYTE)0x0097D9E0, (PBYTE)&hk_LiveSteam_Init);

	MessageBoxA(nullptr, "a", "a", 0);
	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		return ServerMod_Init();
	}

	return TRUE;
}

/*
Address        Function                                                                                                                                                                                       Instruction
-------        --------                                                                                                                                                                                       -----------
.text:0042781F ?Phys_Vec3ToNitrousVec@@YAXQBMAAVphys_vec3@@@Z                                                                                                                                                 mov     eax, 1
.text:0043CC3F ?Phys_NitrousVecToVec3@@YAXABVphys_vec3@@QAM@Z                                                                                                                                                 mov     eax, 1
.text:0050BE4C ?UI_SetLocalVarStringByName@@YAXPBD0H@Z                                                                                                                                                        mov     eax, 1
.text:00562ADD ?CL_AllocatePerLocalClientMemory@@YAXPBDI@Z                                                                                                                                                    mov     eax, 1
.text:0058F0FA DB_Sleep                                                                                                                                                                                       mov     eax, 1
.text:00594EED ?DB_LoadFastFilesForPC@@YAXXZ                                                                                                                                                                  mov     eax, 1
.text:00594F5C ?DB_LoadFastFilesForPC@@YAXXZ                                                                                                                                                                  mov     eax, 1
.text:00596766 DevGui_CreateMenu                                                                                                                                                                              mov     eax, 1
.text:0059EE14 ?dwLogOnComplete@@YAXH@Z                                                                                                                                                                       mov     eax, 1
.text:005A0203 ?DWDedicatedLogon@@YAXXZ                                                                                                                                                                       mov     eax, 1
.text:005A1005 ?dwUpdateSessionComplete@@YA?AW4taskCompleteResults@@QAUoverlappedTask@@@Z                                                                                                                     mov     eax, 1
.text:0060D57B PlayerCmd_IsLocalToHost                                                                                                                                                                        mov     eax, 1
.text:0061155E ?G_Say@@YAXPAUgentity_s@@0HPBD@Z                                                                                                                                                               mov     eax, 1
.text:0066F4E2 ?VEH_Teleport@@YAXPAUgentity_s@@QAM11@Z                                                                                                                                                        mov     eax, 1
.text:006CD654 ?Com_ControllerIndexes_GetPrimary@@YAHXZ                                                                                                                                                       mov     eax, 1
.text:006D4104 ?Com_InitUIAndCommonXAssets@@YAXXZ                                                                                                                                                             mov     eax, 1
.text:006D433B Com_ErrorCleanup                                                                                                                                                                               mov     eax, 1
.text:006D49B8 Com_Init_Try_Block_Function                                                                                                                                                                    mov     eax, 1
.text:006D5003 COM_PlayIntroMovies                                                                                                                                                                            mov     eax, 1
.text:006D5240 Com_InitDvars                                                                                                                                                                                  mov     eax, 1
.text:006D5A85 Com_WriteConfigToFile                                                                                                                                                                          mov     eax, 1
.text:006D5BE8 Com_WriteKeyConfigToFile                                                                                                                                                                       mov     eax, 1
.text:006D5DC6 Com_LoadUiFastFile                                                                                                                                                                             mov     eax, 1
.text:006D5F04 ?Com_LoadMapLoadingScreenFastFile@@YAXPBD@Z                                                                                                                                                    mov     eax, 1
.text:006D6E20 Com_Frame_Try_Block_Function                                                                                                                                                                   mov     eax, 1
.text:006D6FCC Com_ModifyMsec                                                                                                                                                                                 mov     eax, 1
.text:006DD966 GDT_RemoteXModelUpdate                                                                                                                                                                         mov     eax, 1
.text:006DDDBC GDT_RemoteMaterialUpdate                                                                                                                                                                       mov     eax, 1
.text:006E2F63 ?MSG_GetMapCenter@@YAPAY02MXZ                                                                                                                                                                  mov     eax, 1
.text:006F5873 ?Sys_WaitRenderer@@YAHXZ                                                                                                                                                                       mov     eax, 1
.text:00703AF4 ?SV_AddOperatorCommands@@YAXXZ                                                                                                                                                                 mov     eax, 1
.text:00707816 ?SV_DirectConnect@@YAXUnetadr_t@@@Z                                                                                                                                                            mov     eax, 1
.text:0070F5F8 SV_InitGameVM                                                                                                                                                                                  mov     eax, 1
.text:007101D3 ?SV_Startup@@YAXH@Z                                                                                                                                                                            mov     eax, 1
.text:00710C43 ?SV_SpawnServer@@YAXHPBDHH@Z                                                                                                                                                                   mov     eax, 1
.text:00711908 ?SV_Init@@YAXXZ                                                                                                                                                                                mov     eax, 1
.text:00713D2B ?SVC_Info@@YAXUnetadr_t@@PAVbdSecurityID@@_N@Z                                                                                                                                                 mov     eax, 1
.text:00716B81 ?SV_MasterHeartbeat@@YAXHPBD@Z                                                                                                                                                                 mov     eax, 1
.text:007179A3 ?SV_MatchEnd@@YAXXZ                                                                                                                                                                            mov     eax, 1
.text:0073EB54 ?UI_Gametype_IsUsingCustom@@YA_NXZ                                                                                                                                                             mov     eax, 1
.text:00752954 ?UI_DrawConnectScreen@@YAXH@Z                                                                                                                                                                  mov     eax, 1
.text:007631D3 Script_Play                                                                                                                                                                                    mov     eax, 1
.text:00776974 ?UI_SetLoadingScreenMaterial@@YAXPBD@Z                                                                                                                                                         mov     eax, 1
.text:007949C4 CanRenderClip                                                                                                                                                                                  mov     eax, 1
.text:007A79C6 ?FS_Restart@@YAXHH@Z                                                                                                                                                                           mov     eax, 1
.text:007D3F55 KeyValueToField                                                                                                                                                                                mov     eax, 1          ; jumptable 007D3DB3 cases 11,12
.text:007D99CE ?Sys_Error@@YAXPBDZZ                                                                                                                                                                           mov     eax, 1
.text:007DB6C5 _WinMain@16                                                                                                                                                                                    mov     eax, 1
.text:007E7F89 ?DObjCalcSkel@@YAXPBUDObj@@QAH@Z                                                                                                                                                               mov     eax, 1
.text:0081F122 ?init@NitrousVehicle@@QAEXPAUgentity_s@@PBVVehicleParameter@@@Z                                                                                                                                mov     eax, 1
.text:0081F156 ?init@NitrousVehicle@@QAEXPAUgentity_s@@PBVVehicleParameter@@@Z                                                                                                                                mov     eax, 1
.text:0081F1BB ?init@NitrousVehicle@@QAEXPAUgentity_s@@PBVVehicleParameter@@@Z                                                                                                                                mov     eax, 1
.text:0081F827 ?init@NitrousVehicle@@QAEXHPAUcentity_s@@PBVVehicleParameter@@@Z                                                                                                                               mov     eax, 1
.text:0081F85B ?init@NitrousVehicle@@QAEXHPAUcentity_s@@PBVVehicleParameter@@@Z                                                                                                                               mov     eax, 1
.text:0081F8C0 ?init@NitrousVehicle@@QAEXHPAUcentity_s@@PBVVehicleParameter@@@Z                                                                                                                               mov     eax, 1
.text:008202A5 ?unpause_physics@NitrousVehicle@@QAEXXZ                                                                                                                                                        mov     eax, 1
.text:0082040D ?pause_physics@NitrousVehicle@@QAEX_N@Z                                                                                                                                                        mov     eax, 1
.text:008211B5 ?update_parms@NitrousVehicle@@QAEXPBVVehicleParameter@@_N@Z                                                                                                                                    mov     eax, 1
.text:008213BE ?update_parms@NitrousVehicle@@QAEXPBVVehicleParameter@@_N@Z                                                                                                                                    mov     eax, 1
.text:008242D2 ?_update_prolog@NitrousVehicle@@QAEXM@Z                                                                                                                                                        mov     eax, 1
.text:00825171 ?start_path@NitrousVehicle@@QAEXH@Z                                                                                                                                                            mov     eax, 1
.text:008253CE ?start_path@NitrousVehicle@@QAEXH@Z                                                                                                                                                            mov     eax, 1
.text:00825402 ?start_path@NitrousVehicle@@QAEXH@Z                                                                                                                                                            mov     eax, 1
.text:00825467 ?start_path@NitrousVehicle@@QAEXH@Z                                                                                                                                                            mov     eax, 1
.text:008255E2 ?start_path@NitrousVehicle@@QAEXH@Z                                                                                                                                                            mov     eax, 1
.text:00828342 ?update_from_network@NitrousVehicle@@QAEXUtrajectory_t@@0PAULerpEntityStateVehicle@@M@Z                                                                                                        mov     eax, 1
.text:00828387 ?update_from_network@NitrousVehicle@@QAEXUtrajectory_t@@0PAULerpEntityStateVehicle@@M@Z                                                                                                        mov     eax, 1
.text:008283FB ?update_from_network@NitrousVehicle@@QAEXUtrajectory_t@@0PAULerpEntityStateVehicle@@M@Z                                                                                                        mov     eax, 1
.text:00828AA1 ?make_rotate@@YAXAAVphys_mat44@@ABVphys_vec3@@MM@Z                                                                                                                                             mov     eax, 1
.text:00828B83 ?make_rotate@@YAXAAVphys_mat44@@ABVphys_vec3@@MM@Z                                                                                                                                             mov     eax, 1
.text:00828BB7 ?make_rotate@@YAXAAVphys_mat44@@ABVphys_vec3@@MM@Z                                                                                                                                             mov     eax, 1
.text:00828C15 ?make_rotate@@YAXAAVphys_mat44@@ABVphys_vec3@@MM@Z                                                                                                                                             mov     eax, 1
.text:008312D9 ?set_geom_id_new@gjk_base_t@@QAEXI@Z                                                                                                                                                           mov     eax, 1
.text:00832A09 ?set_contents@gjk_base_t@@QAEXH@Z                                                                                                                                                              mov     eax, 1
.text:00838209 ?set_rb2_entity@contact_point_info@@QAEXPBX@Z                                                                                                                                                  mov     eax, 1
.text:00839217 ?set@broad_phase_info@@QAEXPAVrigid_body@@PBVphys_mat44@@11PBVphys_gjk_geom@@I_NHPAXI@Z                                                                                                        mov     eax, 1
.text:008482E3 ?environment_collision_list_add@@YAXPAVbroad_phase_base@@@Z                                                                                                                                    mov     eax, 1
.text:0084ADED ?Phys_ObjSetPosition@@YAXHQBM@Z                                                                                                                                                                mov     eax, 1
.text:0084B1FF ?Phys_ObjSetPosition@@YAXHQBM@Z                                                                                                                                                                mov     eax, 1
.text:0084B76A ?Phys_ObjSetOrientation@@YAXHQBM0@Z                                                                                                                                                            mov     eax, 1
.text:0084BA5C ?Phys_ObjSetOrientation@@YAXHQBM0@Z                                                                                                                                                            mov     eax, 1
.text:0085488D ?remove@?$phys_link_list1@UPhysObjUserData@@@@QAEXPAUPhysObjUserData@@@Z                                                                                                                       mov     eax, 1
.text:00855E8B ?add_brush@debug_brush_info_t@@QAEXPBUcbrush_t@@PBVphys_mat44@@@Z                                                                                                                              mov     eax, 1
.text:00856173 ?add_brush@debug_brush_info_t@@QAEXPBUcbrush_t@@PBVphys_mat44@@@Z                                                                                                                              mov     eax, 1
.text:008692C9 Ragdoll_CreatePhysObj                                                                                                                                                                          mov     eax, 1
.text:008693AF Ragdoll_CreatePhysObj                                                                                                                                                                          mov     eax, 1
.text:0086A447 Ragdoll_TunnelTest                                                                                                                                                                             mov     eax, 1
.text:00936076 ?SNDL_Update@@YAXXZ                                                                                                                                                                            mov     eax, 1
.text:00936E66 ?SND_StopVoice@@YAXH@Z                                                                                                                                                                         mov     eax, 1
.text:00947233 ?SNDL_AliasName@@YAXPBDI@Z                                                                                                                                                                     mov     eax, 1
.text:00947286 ?SNDL_Play@@YAHIHMTSndEntHandle@@QBM1_NPAUsnd_playback@@@Z                                                                                                                                     mov     eax, 1
.text:009474B3 ?SNDL_StopSoundAliasOnEnt@@YAXTSndEntHandle@@I@Z                                                                                                                                               mov     eax, 1
.text:00947623 ?SNDL_StopSoundsOnEnt@@YAXTSndEntHandle@@@Z                                                                                                                                                    mov     eax, 1
.text:00947673 ?SNDL_NotifyCinematicStart@@YAXM@Z                                                                                                                                                             mov     eax, 1
.text:009476E3 ?SNDL_NotifyCinematicEnd@@YAXXZ                                                                                                                                                                mov     eax, 1
.text:00947746 ?SNDL_DisconnectListener@@YAXH@Z                                                                                                                                                               mov     eax, 1
.text:009478D9 ?SNDL_SetListener@@YAXHHW4team_t@@QBMQAY02$$CBM@Z                                                                                                                                              mov     eax, 1
.text:00948046 ?SNDL_FadeOut@@YAXXZ                                                                                                                                                                           mov     eax, 1
.text:009480B6 ?SNDL_FadeIn@@YAXXZ                                                                                                                                                                            mov     eax, 1
.text:00948126 ?SNDL_SetEnvironmentEffects@@YAXHIMMH@Z                                                                                                                                                        mov     eax, 1
.text:00948376 ?SNDL_DeactivateEnvironmentEffects@@YAXHH@Z                                                                                                                                                    mov     eax, 1
.text:00948536 ?SNDL_SetPlaybackAttenuation@@YAXHM@Z                                                                                                                                                          mov     eax, 1
.text:00948606 ?SNDL_SetPlaybackAttenuationRate@@YAXHM@Z                                                                                                                                                      mov     eax, 1
.text:00948676 ?SNDL_SetPlaybackPitch@@YAXHM@Z                                                                                                                                                                mov     eax, 1
.text:00948746 ?SNDL_SetPlaybackPitchRate@@YAXHM@Z                                                                                                                                                            mov     eax, 1
.text:009487B4 ?SNDL_StopPlayback@@YAXH@Z                                                                                                                                                                     mov     eax, 1
.text:00948824 ?SNDL_SetSnapshot@@YAXW4snd_snapshot_type@@IMM@Z                                                                                                                                               mov     eax, 1
.text:00948923 ?SNDL_SetGameState@@YAX_N0MII@Z                                                                                                                                                                mov     eax, 1
.text:00948A06 ?SNDL_PlayLoopAt@@YAXIQBM@Z                                                                                                                                                                    mov     eax, 1
.text:00948AE6 ?SNDL_StopLoopAt@@YAXIQBM@Z                                                                                                                                                                    mov     eax, 1
.text:00948C66 ?SNDL_PlayLineAt@@YAXIQBM0@Z                                                                                                                                                                   mov     eax, 1
.text:00948D86 ?SNDL_StopLineAt@@YAXIQBM0@Z                                                                                                                                                                   mov     eax, 1
.text:009495E6 ?SND_LogSkip@@YA_NIIPAI0@Z                                                                                                                                                                     mov     eax, 1
.text:0094B15E ?SND_Play@@YAXPBDHMTSndEntHandle@@QBM2_N@Z                                                                                                                                                     mov     eax, 1
.text:0094B24F ?SND_StopSoundAliasOnEnt@@YAXTSndEntHandle@@I@Z                                                                                                                                                mov     eax, 1
.text:0094B2CF ?SND_StopSoundsOnEnt@@YAXTSndEntHandle@@@Z                                                                                                                                                     mov     eax, 1
.text:0094B33F ?SND_NotifyCinematicStart@@YAXM@Z                                                                                                                                                              mov     eax, 1
.text:0094B3BF ?SND_NotifyCinematicEnd@@YAXXZ                                                                                                                                                                 mov     eax, 1
.text:0094B42F ?SND_DisconnectListener@@YAXH@Z                                                                                                                                                                mov     eax, 1
.text:0094B4A4 ?SND_SetListener@@YAXHHW4team_t@@QBMQAY02$$CBM@Z                                                                                                                                               mov     eax, 1
.text:0094B64F ?SND_StopSounds@@YAXW4snd_stop_sound_flags@@@Z                                                                                                                                                 mov     eax, 1
.text:0094B6CF ?SND_FadeIn@@YAXXZ                                                                                                                                                                             mov     eax, 1
.text:0094B73F ?SND_FadeOut@@YAXXZ                                                                                                                                                                            mov     eax, 1
.text:0094B7B4 ?SND_SetEnvironmentEffects@@YAXHPBDMMH@Z                                                                                                                                                       mov     eax, 1
.text:0094B89F ?SND_DeactivateEnvironmentEffects@@YAXHH@Z                                                                                                                                                     mov     eax, 1
.text:0094BC5F ?SND_StopPlayback@@YAXH@Z                                                                                                                                                                      mov     eax, 1
.text:0094C572 ?SND_SetContext@@YAXPBD0@Z                                                                                                                                                                     mov     eax, 1
.text:0094C67F ?SND_SetScriptTimescale@@YAXM@Z                                                                                                                                                                mov     eax, 1
.text:0094C6F6 ?SND_FindEntState@@YAPAUsnd_ent_state@@TSndEntHandle@@_N@Z                                                                                                                                     mov     eax, 1
.text:0094CB06 ?SND_UpdateEntState@@YAXTSndEntHandle@@QBM1QAY02$$CBM@Z                                                                                                                                        mov     eax, 1
.text:0094CF06 ?SND_GetEntState@@YA_NTSndEntHandle@@QAM1QAY02M@Z                                                                                                                                              mov     eax, 1
.text:0094CFD3 ?SNDL_SetEntState@@YAXTSndEntHandle@@QBM1QAY02$$CBM@Z                                                                                                                                          mov     eax, 1
.text:0094D026 ?SND_EntStateFrame@@YAXXZ                                                                                                                                                                      mov     eax, 1
.text:0094D474 ?SND_EntStateRequest@@YAXTSndEntHandle@@@Z                                                                                                                                                     mov     eax, 1
.text:0094D4E4 ?SND_SubtitleNotify@@YAXPBDI@Z                                                                                                                                                                 mov     eax, 1
.text:0094D554 ?SND_LengthNotify@@YAXII@Z                                                                                                                                                                     mov     eax, 1
.text:0094D5C4 ?SND_FreePlaybackNotify@@YAXPAUsnd_playback@@@Z                                                                                                                                                mov     eax, 1
.text:0094D696 ?SND_AllocatePlayback@@YAPAUsnd_playback@@XZ                                                                                                                                                   mov     eax, 1
.text:0094D7A3 ?SND_FreePlayback@@YAXPAUsnd_playback@@@Z                                                                                                                                                      mov     eax, 1
.text:0094D866 SND_FindPlayback                                                                                                                                                                               mov     eax, 1
.text:0094DBBF ?SND_GameReset@@YAXXZ                                                                                                                                                                          mov     eax, 1
.text:0094DC32 ?SND_BeginFrame@@YAX_N0MII@Z                                                                                                                                                                   mov     eax, 1
.text:0094DC61 ?SND_BeginFrame@@YAX_N0MII@Z                                                                                                                                                                   mov     eax, 1
.text:0094F096 ?SND_NotifyPush@@YAXPAUsnd_notify@@@Z                                                                                                                                                          mov     eax, 1
.text:0094F314 ?SND_NotifyPeek@@YAPBUsnd_notify@@XZ                                                                                                                                                           mov     eax, 1
.text:0094F413 ?SND_NotifyPop@@YAXXZ                                                                                                                                                                          mov     eax, 1
.text:00950ED6 ?Snd_StreamFini@@YAXXZ                                                                                                                                                                         mov     eax, 1
.text:00951815 ?Snd_StreamStatus@@YA?AW4snd_stream_status@@I@Z                                                                                                                                                mov     eax, 1
.text:00951A65 ?Snd_StreamGetFreeWindows@@YAII@Z                                                                                                                                                              mov     eax, 1
.text:00951C85 ?Snd_StreamAcquireWindow@@YA?AW4snd_stream_status@@IPAI0PAPAD@Z                                                                                                                                mov     eax, 1
.text:00962F8E Live_FileShare_Read_f                                                                                                                                                                          mov     eax, 1
.text:00969484 ?LB_SetGametypeDvar@@YA_NXZ                                                                                                                                                                    mov     eax, 1
.text:00972873 ?Session_IsHost@@YA_NPAUSessionData_s@@H@Z                                                                                                                                                     mov     eax, 1
.text:00974D49 ?LiveStats_GetIntPlayerStatInternal@@YA_NPAHHQAPBDPAD@Z                                                                                                                                        mov     eax, 1
.text:00975290 ?LiveStats_SetIntPlayerStatInternal@@YA_NHHQAPBDPADI@Z                                                                                                                                         mov     eax, 1
.text:00977735 LiveStats_GetItemStat                                                                                                                                                                          mov     eax, 1
.text:0097D576 ?LiveSteam_CheckAccess@@YAXXZ                                                                                                                                                                  mov     eax, 1
.text:0097DA0D ?LiveSteam_Init@@YAXXZ                                                                                                                                                                         mov     eax, 1
.text:0097DDBD ?LiveSteam_Frame@@YAXXZ                                                                                                                                                                        mov     eax, 1
.text:0097FA7E ?Register@?$CCallback@VLiveSteamServer@@USteamServersConnected_t@@$00@@QAEXPAVLiveSteamServer@@P82@AEXPAUSteamServersConnected_t@@@Z@Z                                                         mov     eax, 1
.text:0097FAEE ?Register@?$CCallback@VLiveSteamServer@@USteamServersDisconnected_t@@$00@@QAEXPAVLiveSteamServer@@P82@AEXPAUSteamServersDisconnected_t@@@Z@Z                                                   mov     eax, 1
.text:0097FB5E ?Register@?$CCallback@VLiveSteamServer@@UGSPolicyResponse_t@@$00@@QAEXPAVLiveSteamServer@@P82@AEXPAUGSPolicyResponse_t@@@Z@Z                                                                   mov     eax, 1
.text:0097FBCE ?Register@?$CCallback@VLiveSteamServer@@UGSClientApprove_t@@$00@@QAEXPAVLiveSteamServer@@P82@AEXPAUGSClientApprove_t@@@Z@Z                                                                     mov     eax, 1
.text:0097FC4E ?Register@?$CCallback@VLiveSteamServer@@UGSClientDeny_t@@$00@@QAEXPAVLiveSteamServer@@P82@AEXPAUGSClientDeny_t@@@Z@Z                                                                           mov     eax, 1
.text:0097FCCE ?Register@?$CCallback@VLiveSteamServer@@UGSClientKick_t@@$00@@QAEXPAVLiveSteamServer@@P82@AEXPAUGSClientKick_t@@@Z@Z                                                                           mov     eax, 1
.text:0098142A ?LiveStorage_ReadDWFileByUserID@@YAPAUTaskRecord@@HPAUdwFileOperationInfo@@_K@Z                                                                                                                mov     eax, 1
.text:009818D6 ?LiveStorage_GetServerTimeComplete@@YAXPAUTaskRecord@@@Z                                                                                                                                       mov     eax, 1
.text:00983858 ?LiveStorage_FileShare_ReadListingSuccess@@YAXPAUTaskRecord@@@Z                                                                                                                                mov     eax, 1
.text:0098517C ?LiveStorage_FileShare_WriteSummarySuccess@@YAXPAUTaskRecord@@@Z                                                                                                                               mov     eax, 1
.text:00986BD1 ?LiveStorage_Init@@YA_NXZ                                                                                                                                                                      mov     eax, 1
.text:00987B06 LiveStorage_ProcessOnlineWAD                                                                                                                                                                   mov     eax, 1
.text:0098971A ?SV_CommitClientLeaderboards@@YAXXZ                                                                                                                                                            mov     eax, 1
.text:0098B0B6 ?SV_DWWriteClientStats@@YAXPAUclient_t@@@Z                                                                                                                                                     mov     eax, 1
.text:0098C6B6 ?SV_CACValidateWriteCAC@@YAX_KPAEI@Z                                                                                                                                                           mov     eax, 1
.text:0098C896 ?SV_CACValidateWriteGlobal@@YAX_KPAEI@Z                                                                                                                                                        mov     eax, 1
.text:00991484 ?Live_Frame@@YAXHH@Z                                                                                                                                                                           mov     eax, 1
.text:00999E96 ?GenerateVerts@GlassRenderer@@QAEXHII@Z                                                                                                                                                        mov     eax, 1
.text:0099B2B2 ?GetSmallestShards@GlassRenderer@@QAEXI_N0@Z                                                                                                                                                   mov     eax, 1
.text:0099B402 ?GetLargestShards@GlassRenderer@@QAEXI_N0@Z                                                                                                                                                    mov     eax, 1
.text:0099CF71 ?DrawDebug@GlassRenderer@@QAEXXZ                                                                                                                                                               mov     eax, 1
.text:0099E1AF ?back@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QAEAAPAUGlassShard@@XZ                                                                                            mov     eax, 1
.text:0099E20F ?pop_back@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QAEXXZ                                                                                                        mov     eax, 1
.text:0099E409 ??D?$_Const_iterator@$00@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QBEABQAUGlassShard@@XZ                                                                         mov     eax, 1
.text:0099E497 ??9?$_Const_iterator@$00@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QBE_NABV012@@Z                                                                                 mov     eax, 1
.text:0099E709 ??E?$_Const_iterator@$00@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAPAUGlassShard@@@@@std@@QAEAAV012@XZ                                                                                 mov     eax, 1
.text:0099E797 ??8?$_Const_iterator@$00@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QBE_NABV012@@Z                                                                                 mov     eax, 1
.text:0099E809 ??0?$_Const_iterator@$00@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QAE@PAU_Node@?$_List_nod@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@2@PBV12@@Z mov     eax, 1
.text:0099E849 ??F?$_Const_iterator@$00@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QAEAAV012@XZ                                                                                   mov     eax, 1
.text:0099E87C ??F?$_Const_iterator@$00@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QAEAAV012@XZ                                                                                   mov     eax, 1
.text:0099E9EA ?Allocate@?$FixedSizeAllocator@PAUShardGroup@@@@QAEPAUShardGroup@@XZ                                                                                                                           mov     eax, 1
.text:0099EC2B ?Free@?$FixedSizeAllocator@PAUGlassShard@@@@QAEXPAUGlassShard@@@Z                                                                                                                              mov     eax, 1
.text:0099ECB7 ?Free@?$FixedSizeAllocator@PAUGlassShard@@@@QAEXPAUGlassShard@@@Z                                                                                                                              mov     eax, 1
.text:0099EE23 ?FreeAll@?$FixedSizeAllocator@PAUGlassShard@@@@QAEXXZ                                                                                                                                          mov     eax, 1
.text:0099F43D ?IsValidUsedPtr@?$FixedSizeAllocator@PAUGlassPhysics@@@@QBE_NPAUGlassPhysics@@@Z                                                                                                               mov     eax, 1
.text:009AB58C ?Split@GlassShard@@QAEHQAPAU1@MIM@Z                                                                                                                                                            mov     eax, 1
.text:009D3527 sub_9D3510                                                                                                                                                                                     mov     eax, 1
.text:00BB9DE6 sub_BB9DE0                                                                                                                                                                                     mov     eax, 1
.text:00BBAB96 sub_BBAB90                                                                                                                                                                                     mov     eax, 1
.text:00BCFCBD sub_BCFC90                                                                                                                                                                                     mov     eax, 1
.text:00BD3DE7 sub_BD3C40                                                                                                                                                                                     mov     eax, 1
.text:00BD9F36 sub_BD9F30                                                                                                                                                                                     mov     eax, 1
.text:00BDC8DA sub_BDC850                                                                                                                                                                                     mov     eax, 1
.text:00BE1C8A _HTNextField                                                                                                                                                                                   mov     eax, 1
.text:00BE1F8A _HTNextPair                                                                                                                                                                                    mov     eax, 1
.text:00BE21DC _HTNextElement                                                                                                                                                                                 mov     eax, 1

Address        Function                                                                                                                            Instruction
-------        --------                                                                                                                            -----------
.text:00439A23 ?Phys_Vec3ToNitrousVec@@YA?BVphys_vec3@@QBM@Z                                                                                       mov     edx, 1
.text:00440374 ?comp_aabb_loc@gjk_base_t@@UAEXXZ                                                                                                   mov     edx, 1
.text:004B3EC6 ?CG_EntityEvent@@YAXHPAUcentity_s@@H@Z                                                                                              mov     edx, 1
.text:004B7DC6 ?CG_PhysLaunch@@YAXHPAUcentity_s@@PBUentityState_s@@@Z                                                                              mov     edx, 1
.text:00594220 DB_SyncExternalAssets                                                                                                               mov     edx, 1
.text:005970DB ?DevGui_AddGraph@@YAXPBDPAUDevGraph@@@Z                                                                                             mov     edx, 1
.text:005F4FAB Bullet_ImpactEffect                                                                                                                 mov     edx, 1
.text:0061149D ?G_Say@@YAXPAUgentity_s@@0HPBD@Z                                                                                                    mov     edx, 1
.text:0066F06D ?VEH_Teleport@@YAXPAUgentity_s@@QAM11@Z                                                                                             mov     edx, 1
.text:0066F0A1 ?VEH_Teleport@@YAXPAUgentity_s@@QAM11@Z                                                                                             mov     edx, 1
.text:0066F100 ?VEH_Teleport@@YAXPAUgentity_s@@QAM11@Z                                                                                             mov     edx, 1
.text:0066F462 ?VEH_Teleport@@YAXPAUgentity_s@@QAM11@Z                                                                                             mov     edx, 1
.text:006D194B ?Com_PrintMessage@@YAXHPBDH@Z                                                                                                       mov     edx, 1
.text:006D24D7 ?Com_Error@@YAXW4errorParm_t@@PBDZZ                                                                                                 mov     edx, 1
.text:006D6972 Com_Frame_Try_Block_Function                                                                                                        mov     edx, 1
.text:006D6C66 Com_Frame_Try_Block_Function                                                                                                        mov     edx, 1
.text:0070F654 SV_InitGameVM                                                                                                                       mov     edx, 1
.text:007101B2 ?SV_Startup@@YAXH@Z                                                                                                                 mov     edx, 1
.text:0071020B ?SV_Startup@@YAXH@Z                                                                                                                 mov     edx, 1
.text:00712DED ?SV_SendServerCommand@@YAXPAUclient_t@@W4svscmd_type@@PBDZZ                                                                         mov     edx, 1
.text:00716827 ?SV_MasterHeartbeat@@YAXHPBD@Z                                                                                                      mov     edx, 1
.text:0071BDC0 ?SV_AddModifiedStats@@YAXH@Z                                                                                                        mov     edx, 1
.text:0073E836 ?UI_Gametype_ReadFromMemFile@@YA_NAAUMemoryFile@@@Z                                                                                 mov     edx, 1
.text:00740BFB ?UI_Gametype_FileShareDownloadComplete@@YAXPAUdwFileShareReadFileTask@@@Z                                                           mov     edx, 1
.text:007841D9 ?AddPlayerToScene@UIViewer@@QAEXABUPlayerParams@1@ABUWeaponParams@1@_N@Z                                                            mov     edx, 1
.text:007D3EE4 KeyValueToField                                                                                                                     mov     edx, 1          ; jumptable 007D3DB3 case 9
.text:007D9EE1 Win_GetEvent                                                                                                                        mov     edx, 1
.text:007DB72D _WinMain@16                                                                                                                         mov     edx, 1
.text:00823F7E ?_update_prolog@NitrousVehicle@@QAEXM@Z                                                                                             mov     edx, 1
.text:0082432B ?_update_prolog@NitrousVehicle@@QAEXM@Z                                                                                             mov     edx, 1
.text:00824467 ?_update_prolog@NitrousVehicle@@QAEXM@Z                                                                                             mov     edx, 1
.text:00828D7C ?make_rotate@@YAXAAVphys_mat44@@ABVphys_vec3@@MM@Z                                                                                  mov     edx, 1
.text:0082A93F ?UpdateScriptVehicleControl@NitrousVehicleController@@AAEXAAVNitrousVehicle@@M@Z                                                    mov     edx, 1
.text:00831145 ?create@gjk_aabb_t@@SAPAU1@ABVphys_vec3@@0HPAVgjk_collision_visitor@@@Z                                                             mov     edx, 1
.text:008313B5 ?support@gjk_aabb_t@@UBEXABVphys_vec3@@PAV2@1@Z                                                                                     mov     edx, 1
.text:008328B8 ?create@gjk_brush_t@@SAPAUgjk_base_t@@PBUcbrush_t@@HPAVgjk_collision_visitor@@@Z                                                    mov     edx, 1
.text:00833FF6 ?create@gjk_double_sphere_t@@SAPAU1@ABVphys_vec3@@0MHPAVgjk_collision_visitor@@@Z                                                   mov     edx, 1
.text:00839378 ?set_bpi_env@broad_phase_info@@QAEXPAVphys_auto_activate_callback@@@Z                                                               mov     edx, 1
.text:0083BE60 ?debug_callback@@YAXPAX@Z                                                                                                           mov     edx, 1
.text:0083C2A2 ?collide_vehicle_wheels@@YAXAAUPhysObjUserData@@@Z                                                                                  mov     edx, 1
.text:0083F03E ?Phys_EffectsProcess@@YAXXZ                                                                                                         mov     edx, 1
.text:008480C9 ?set@broad_phase_group@@QAEXXZ                                                                                                      mov     edx, 1
.text:0084AE2C ?Phys_ObjSetPosition@@YAXHQBM@Z                                                                                                     mov     edx, 1
.text:0084B14A ?Phys_ObjSetPosition@@YAXHQBM@Z                                                                                                     mov     edx, 1
.text:0084B6BD ?Phys_ObjSetOrientation@@YAXHQBM0@Z                                                                                                 mov     edx, 1
.text:0084BACF ?Phys_ObjSetOrientation@@YAXHQBM0@Z                                                                                                 mov     edx, 1
.text:0085934B ?init_winding@@YAXABUplane_lt@@AAV?$phys_static_array@Vphys_vec3@@$0CAA@@@@Z                                                        mov     edx, 1
.text:0086A489 Ragdoll_TunnelTest                                                                                                                  mov     edx, 1
.text:0086A6C0 Ragdoll_TunnelTest                                                                                                                  mov     edx, 1
.text:009372DE ?SND_Init@@YAXXZ                                                                                                                    mov     edx, 1
.text:0094971C ?SND_LogMissingAliasId@@YAXI@Z                                                                                                      mov     edx, 1
.text:0094ABC8 ?SND_PlayInternal@@YAXIHMTSndEntHandle@@QBM1_NPAUsnd_playback@@@Z                                                                   mov     edx, 1
.text:0094BFA8 ?SND_PlayLoopAt@@YAXIQBM@Z                                                                                                          mov     edx, 1
.text:0094C0E8 ?SND_StopLoopAt@@YAXIQBM@Z                                                                                                          mov     edx, 1
.text:0094E327 ?SND_CommandPump@@YAIXZ                                                                                                             mov     edx, 1
.text:0094F594 ?SND_NotifyPump@@YAXXZ                                                                                                              mov     edx, 1
.text:0095119C ?Snd_StreamOpen@@YAXIPBD_NIPAD@Z                                                                                                    mov     edx, 1
.text:009515AC ?Snd_StreamClose@@YAXI@Z                                                                                                            mov     edx, 1
.text:00962FDD Live_FileShare_Read_f                                                                                                               mov     edx, 1
.text:0096F93E ?PCache_GetEntry@@YAPAUPCacheEntry@@H_K@Z                                                                                           mov     edx, 1
.text:0098CB1B ?SV_CACValidateReadCAC@@YAPAUTaskRecord@@_KPAEI@Z                                                                                   mov     edx, 1
.text:0098CEFC ?SV_CACValidateReadGlobal@@YAPAUTaskRecord@@_KPAEI@Z                                                                                mov     edx, 1
.text:00999FB4 ?ExplosionEvent@GlassRenderer@@AAEXQBMMMMH@Z                                                                                        mov     edx, 1
.text:0099A0CB ?TracePoint@GlassRenderer@@AAEHQBM0@Z                                                                                               mov     edx, 1
.text:0099A585 ?GetShardGroup@GlassRenderer@@QAEPAUShardGroup@@IPBUGlassDef@@@Z                                                                    mov     edx, 1
.text:0099E157 ?front@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QAEAAPAUGlassShard@@XZ                                mov     edx, 1
.text:0099E374 ?end@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAPAUGlassShard@@@@@std@@QAE?AV?$_Iterator@$00@12@XZ                           mov     edx, 1
.text:0099E42D ??D?$_Const_iterator@$00@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QBEABQAUGlassShard@@XZ              mov     edx, 1
.text:0099E72D ??E?$_Const_iterator@$00@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAPAUGlassShard@@@@@std@@QAEAAV012@XZ                      mov     edx, 1
.text:0099EC6D ?Free@?$FixedSizeAllocator@PAUGlassShard@@@@QAEXPAUGlassShard@@@Z                                                                   mov     edx, 1
.text:0099EFC7 ?push_front@?$list@PAUGlassPhysics@@V?$SmallAllocatorTemplate@PAPAUGlassPhysics@@@@@std@@QAEXABQAUGlassPhysics@@@Z                  mov     edx, 1
.text:0099F229 ?erase@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAUGlassShard@@@@@std@@QAE?AV?$_Iterator@$00@12@V?$_Const_iterator@$00@12@@Z mov     edx, 1
.text:0099F4CD ?IsValidUsedPtr@?$FixedSizeAllocator@PAUGlassPhysics@@@@QBE_NPAUGlassPhysics@@@Z                                                    mov     edx, 1
.text:00A56C3D R_MarkModelCoreCallback_1_                                                                                                          mov     edx, 1
.text:00A747B4 RB_StandardDrawCommands                                                                                                             mov     edx, 1
.text:00B61BB1 _jinit_color_converter                                                                                                              mov     edx, 1
.text:00BBA5FF sub_BBA5F0                                                                                                                          mov     edx, 1
.text:00BBB06D sub_BBB040                                                                                                                          mov     edx, 1
.text:00BC8CE3 _HTDoConnect                                                                                                                        mov     edx, 1
.text:00BC94B3 _HTDoListen                                                                                                                         mov     edx, 1
.text:00BD04D9 sub_BD0400                                                                                                                          mov     edx, 1

Address        Function                                                                                                    Instruction
-------        --------                                                                                                    -----------
.text:0041A9B7 ?BG_EmblemsInit@@YAXXZ                                                                                      mov     ecx, 1
.text:00539E4E ?phys_transpose@@YAXAAVphys_mat44@@ABV1@@Z                                                                  mov     ecx, 1
.text:00539F3A ?phys_transpose@@YAXAAVphys_mat44@@ABV1@@Z                                                                  mov     ecx, 1
.text:00539FC6 ?phys_transpose@@YAXAAVphys_mat44@@ABV1@@Z                                                                  mov     ecx, 1
.text:00539FFA ?phys_transpose@@YAXAAVphys_mat44@@ABV1@@Z                                                                  mov     ecx, 1
.text:0053A060 ?phys_transpose@@YAXAAVphys_mat44@@ABV1@@Z                                                                  mov     ecx, 1
.text:0053A1BA ?phys_transpose@@YAXAAVphys_mat44@@ABV1@@Z                                                                  mov     ecx, 1
.text:005D2549 FX_DrawElement_Setup_1_                                                                                     mov     ecx, 1
.text:006114CA ?G_Say@@YAXPAUgentity_s@@0HPBD@Z                                                                            mov     ecx, 1
.text:0066F411 ?VEH_Teleport@@YAXPAUgentity_s@@QAM11@Z                                                                     mov     ecx, 1
.text:0068D437 ?ConsoleCommand@@YAHXZ                                                                                      mov     ecx, 1
.text:006D1F4F ?Com_Shutdown@@YAXPBD@Z                                                                                     mov     ecx, 1
.text:006D4D23 Com_Init_Try_Block_Function                                                                                 mov     ecx, 1
.text:006D4D8F Com_Init_Try_Block_Function                                                                                 mov     ecx, 1
.text:006D66B3 ?Com_LoadFrontEnd@@YAXXZ                                                                                    mov     ecx, 1
.text:006D68C7 ?Com_Frame@@YAXXZ                                                                                           mov     ecx, 1
.text:006D69EC Com_Frame_Try_Block_Function                                                                                mov     ecx, 1
.text:00703CDC SV_Map_f                                                                                                    mov     ecx, 1
.text:00703F82 ShowLoadErrorsSummary                                                                                       mov     ecx, 1
.text:007077C4 ?SV_DirectConnect@@YAXUnetadr_t@@@Z                                                                         mov     ecx, 1
.text:007083D2 ?SV_DropClient@@YAXPAUclient_t@@PBD_N2@Z                                                                    mov     ecx, 1
.text:0071065F ?SV_SpawnServer@@YAXHPBDHH@Z                                                                                mov     ecx, 1
.text:00711693 ?SV_Init@@YAXXZ                                                                                             mov     ecx, 1
.text:00712186 ?SV_Shutdown@@YAXPBD@Z                                                                                      mov     ecx, 1
.text:007121F2 ?SV_Shutdown@@YAXPBD@Z                                                                                      mov     ecx, 1
.text:00713FA6 ?SVC_Info@@YAXUnetadr_t@@PAVbdSecurityID@@_N@Z                                                              mov     ecx, 1
.text:00714410 ?SV_ConnectionlessPacket@@YAXUnetadr_t@@PAUmsg_t@@@Z                                                        mov     ecx, 1
.text:00716815 ?SV_MasterHeartbeat@@YAXHPBD@Z                                                                              mov     ecx, 1
.text:00716BBC ?SV_MasterHeartbeat@@YAXHPBD@Z                                                                              mov     ecx, 1
.text:0076567F ?Item_ListBox_Viewmax@@YAHHHPAUitemDef_s@@@Z                                                                mov     ecx, 1
.text:0078A546 ?Expression_Parse@@YA_NPAPBDPAUExpressionStatement@@PAXH@Z                                                  mov     ecx, 1
.text:0078AAC7 Expression_ParseOperatorToken                                                                               mov     ecx, 1
.text:0078AF0F ?Expression_Init@@YAXXZ                                                                                     mov     ecx, 1
.text:007DB717 _WinMain@16                                                                                                 mov     ecx, 1
.text:007DCA8A ?dwPlatformInit@@YAHAAVbdNetStartParams@@@Z                                                                 mov     ecx, 1
.text:007DE9F1 ConWndProc                                                                                                  mov     ecx, 1          ; jumptable 007DE8F9 case 11
.text:0081EB9C ??0NitrousVehicle@@QAE@XZ                                                                                   mov     ecx, 1
.text:0082158A ?update_parms@NitrousVehicle@@QAEXPBVVehicleParameter@@_N@Z                                                 mov     ecx, 1
.text:008243E7 ?_update_prolog@NitrousVehicle@@QAEXM@Z                                                                     mov     ecx, 1
.text:0082D29C ?SetScriptTarget@NitrousVehicleController@@QAEXAAVNitrousVehicle@@AAVphys_vec3@@MM_N@Z                      mov     ecx, 1
.text:0082D5A3 ?add_force@rigid_body@@QAEXABVphys_vec3@@@Z                                                                 mov     ecx, 1
.text:0082D5D7 ?add_force@rigid_body@@QAEXABVphys_vec3@@@Z                                                                 mov     ecx, 1
.text:0082D63C ?add_force@rigid_body@@QAEXABVphys_vec3@@@Z                                                                 mov     ecx, 1
.text:00831CFB ?create@gjk_obb_t@@SAPAU1@ABVphys_mat44@@ABVphys_vec3@@HPAVgjk_collision_visitor@@@Z                        mov     ecx, 1
.text:00831E9D ?support@gjk_obb_t@@UBEXABVphys_vec3@@PAV2@1@Z                                                              mov     ecx, 1
.text:00832EBB ?create@gjk_partition_t@@SAPAU1@PBUCollisionAabbTree@@PAVgjk_collision_visitor@@@Z                          mov     ecx, 1
.text:0083435E ?create@gjk_cylinder_t@@SAPAU1@HMMABVphys_mat44@@HPAVgjk_collision_visitor@@@Z                              mov     ecx, 1
.text:00835C3E ?create@gjk_polygon_cylinder_t@@SAPAU1@AAY02$$CBM0MHPAVgjk_collision_visitor@@@Z                            mov     ecx, 1
.text:00835DE2 ?destroy_gjk_geom@@YAXPAUgjk_base_t@@@Z                                                                     mov     ecx, 1          ; jumptable 00835D76 default case
.text:00836F30 ?set_xform@gjk_base_t@@QAEXPBVphys_mat44@@@Z                                                                mov     ecx, 1
.text:0083D1B2 ?collide_vehicle_wheels@@YAXAAUPhysObjUserData@@@Z                                                          mov     ecx, 1
.text:00848161 ?add_bpi@broad_phase_group@@QAEXPAVbroad_phase_info@@@Z                                                     mov     ecx, 1
.text:0084ABCC ?Phys_ObjSetPosition@@YAXHQBM@Z                                                                             mov     ecx, 1
.text:0084AE9A ?Phys_ObjSetPosition@@YAXHQBM@Z                                                                             mov     ecx, 1
.text:0084B18C ?Phys_ObjSetPosition@@YAXHQBM@Z                                                                             mov     ecx, 1
.text:0084B4D6 ?Phys_ObjSetOrientation@@YAXHQBM0@Z                                                                         mov     ecx, 1
.text:0084B6FC ?Phys_ObjSetOrientation@@YAXHQBM0@Z                                                                         mov     ecx, 1
.text:0084BA1A ?Phys_ObjSetOrientation@@YAXHQBM0@Z                                                                         mov     ecx, 1
.text:0084BE20 ?dangerous_set_a_vel@rigid_body@@QAEXABVphys_vec3@@@Z                                                       mov     ecx, 1
.text:0084BE54 ?dangerous_set_a_vel@rigid_body@@QAEXABVphys_vec3@@@Z                                                       mov     ecx, 1
.text:0084BEB9 ?dangerous_set_a_vel@rigid_body@@QAEXABVphys_vec3@@@Z                                                       mov     ecx, 1
.text:0084C270 ?dangerous_set_t_vel@rigid_body@@QAEXABVphys_vec3@@@Z                                                       mov     ecx, 1
.text:0084C2A4 ?dangerous_set_t_vel@rigid_body@@QAEXABVphys_vec3@@@Z                                                       mov     ecx, 1
.text:0084C309 ?dangerous_set_t_vel@rigid_body@@QAEXABVphys_vec3@@@Z                                                       mov     ecx, 1
.text:0084D493 ?add_torque@rigid_body@@QAEXABVphys_vec3@@@Z                                                                mov     ecx, 1
.text:0084D4C7 ?add_torque@rigid_body@@QAEXABVphys_vec3@@@Z                                                                mov     ecx, 1
.text:0084D52C ?add_torque@rigid_body@@QAEXABVphys_vec3@@@Z                                                                mov     ecx, 1
.text:0085940E ?init_winding@@YAXABUplane_lt@@AAV?$phys_static_array@Vphys_vec3@@$0CAA@@@@Z                                mov     ecx, 1
.text:00869346 Ragdoll_CreatePhysObj                                                                                       mov     ecx, 1
.text:0086A4FA Ragdoll_TunnelTest                                                                                          mov     ecx, 1
.text:008E3F5B yylex                                                                                                       mov     ecx, 1
.text:00931104 ?SND_ShouldInit@@YA_NXZ                                                                                     mov     ecx, 1
.text:0094B96A ?SND_SetPlaybackAttenuation@@YAXHM@Z                                                                        mov     ecx, 1
.text:0094BA3A ?SND_SetPlaybackAttenuationRate@@YAXHM@Z                                                                    mov     ecx, 1
.text:0094BB0A ?SND_SetPlaybackPitch@@YAXHM@Z                                                                              mov     ecx, 1
.text:0094BBDA ?SND_SetPlaybackPitchRate@@YAXHM@Z                                                                          mov     ecx, 1
.text:0094BD6D ?SND_SetSnapshot@@YAXW4snd_snapshot_type@@PBDMM@Z                                                           mov     ecx, 1
.text:0094BE7F ?SND_SetEntState@@YAXTSndEntHandle@@@Z                                                                      mov     ecx, 1
.text:0094C2B6 ?SND_PlayLineAt@@YAXIQBM0@Z                                                                                 mov     ecx, 1
.text:0094C4A6 ?SND_StopLineAt@@YAXIQBM0@Z                                                                                 mov     ecx, 1
.text:0096D683 ?LiveNews_PopulateFriendNews@@YAXH_KPAD@Z                                                                   mov     ecx, 1
.text:0096E04F ?LiveNews_GetOwnNews@@YAXH@Z                                                                                mov     ecx, 1
.text:00978364 LiveStats_GetPlayerStat                                                                                     mov     ecx, 1
.text:0098D9DB ?SV_FetchWADDeferred@@YAXXZ                                                                                 mov     ecx, 1
.text:0099119B ?PC_InitSigninState@@YAXXZ                                                                                  mov     ecx, 1
.text:00994876 ?SocketRouter_EmergencyFrame@@YAXPBD@Z                                                                      mov     ecx, 1
.text:009992AC ?Reset@GlassRenderer@@QAEXXZ                                                                                mov     ecx, 1
.text:0099955C ?RemoveGlassShards@GlassRenderer@@QAEXI@Z                                                                   mov     ecx, 1
.text:00999803 ?RemovePhysicsShards@GlassRenderer@@QAEXXZ                                                                  mov     ecx, 1
.text:00999C90 ?Update@GlassRenderer@@QAEXH@Z                                                                              mov     ecx, 1
.text:0099A8CF ?Insert@SortedShardsList@GlassRenderer@@QAEXPAUGlassShard@@@Z                                               mov     ecx, 1
.text:0099ABAF ?InsertReverse@SortedShardsList@GlassRenderer@@QAEXPAUGlassShard@@@Z                                        mov     ecx, 1
.text:0099E3CC ?begin@?$list@PAUGlassShard@@V?$SmallAllocatorTemplate@PAPAUGlassShard@@@@@std@@QAE?AV?$_Iterator@$00@12@XZ mov     ecx, 1
.text:0099ECFA ?Free@?$FixedSizeAllocator@PAUGlassShard@@@@QAEXPAUGlassShard@@@Z                                           mov     ecx, 1
.text:0099F3FB ?IsValidUsedPtr@?$FixedSizeAllocator@PAUGlassPhysics@@@@QBE_NPAUGlassPhysics@@@Z                            mov     ecx, 1
.text:0099F487 ?IsValidUsedPtr@?$FixedSizeAllocator@PAUGlassPhysics@@@@QBE_NPAUGlassPhysics@@@Z                            mov     ecx, 1
.text:009CA813 ?Demo_WriteToStream@@YAIPAXII@Z                                                                             mov     ecx, 1
.text:00A57217 R_AddMarkFragment_1_                                                                                        mov     ecx, 1
.text:00A74410 RB_StandardDrawCommands                                                                                     mov     ecx, 1
.text:00A7A2D7 ?R_IssueRenderCommands@@YAXI@Z                                                                              mov     ecx, 1
.text:00AA64F4 R_StreamUpdateAabbNode_r_0_                                                                                 mov     ecx, 1
.text:00AB7687 RB_RenderCommandFrame                                                                                       mov     ecx, 1
.text:00B6910C _jinit_color_deconverter                                                                                    mov     ecx, 1
.text:00BB921C sub_BB90C0                                                                                                  mov     ecx, 1
.text:00BDB226 sub_BDB1B0                                                                                                  mov     ecx, 1
.text:00BDDA52 sub_BDD9A0                                                                                                  mov     ecx, 1
*/