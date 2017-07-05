#include "stdafx.h"

void Com_RunFrontend()
{
	int v0; // esi@2
	char v1; // al@9
	int localClientNum; // esi@9
	int controllerIndex; // eax@11
	XZoneInfo zoneInfo; // [sp+408h] [bp-Ch]@6
	// 0x38789A0 - xblive_matchEndingSoon
	Dvar_SetBool((dvar_s *)0x38789A0, false);
	func_41FEE0();

	char *currentMapname = (char *)Dvar_GetString("mapname");
	localClientNum = GetLocalClientNum();
	controllerIndex = UI_UIContext_GetInfo(localClientNum);

	Com_Printf(1, "Checking for mod_frontend\n");
	if (PartyClient_CheckMapExists("mod_frontend") && I_strcmp(currentMapname, "mod_frontend") != 0)
	{
		Com_Printf(1, "mod_frontend exists. Loading mod_frontend...\n");
		Cmd_ExecuteSingleCommandInternal(localClientNum, controllerIndex, NULL, "map mod_frontend", false); // doing this until I find a better alternative
		//Cbuf_ExecuteBuffer(localClientNum, controllerIndex, "map mod_frontend\n");
	}
	else if (!PartyClient_CheckMapExists("mod_frontend") && I_strcmp(currentMapname, "frontend") != 0)
	{
		Com_Printf(1, "Loading frontend...\n");
		Cmd_ExecuteSingleCommandInternal(localClientNum, controllerIndex, NULL, "map frontend", false); // doing this until I find a better alternative
		//Cbuf_ExecuteBuffer(localClientNum, controllerIndex, "map frontend\n");
	}
}

void UI_RunMod_HandleFrontend()
{
	int localClientNum; // esi@9
	int controllerIndex; // eax@11
	Com_Printf(1, "Getting GetLocalClientNum value\nsub_5BEE40() = ");
	localClientNum = GetLocalClientNum();	// why is localClientNum being this huge, I dont get it
						// 1703388
	Com_Printf(1, "%d\nGetting UIContext Info\nUI_UIContext_GetInfo() = ");
	controllerIndex = UI_UIContext_GetInfo(localClientNum);
	Cmd_ExecuteSingleCommandInternal(localClientNum, controllerIndex, NULL, "vid_restart_complete", false);
	Cmd_ExecuteSingleCommandInternal(localClientNum, controllerIndex, NULL, "wait", false);
	if (PartyClient_CheckMapExists("mod_frontend"))
	{
		Com_Printf(1, "mod_frontend exists. Loading mod_frontend...\n");
		Cmd_ExecuteSingleCommandInternal(localClientNum, controllerIndex, NULL, "map mod_frontend", false); // doing this until I find a better alternative
																				   //Cbuf_ExecuteBuffer(localClientNum, controllerIndex, "map mod_frontend\n");
	}
	else
	{
		Com_Printf(1, "Loading frontend...\n");
		Cmd_ExecuteSingleCommandInternal(localClientNum, controllerIndex, NULL, "map frontend", false); // doing this until I find a better alternative
																			   //Cbuf_ExecuteBuffer(localClientNum, controllerIndex, "map frontend\n");
	}
}

void Mod_Frontend_Init()
{
#define PatchRunFrontend(addr, jmp) Detours::X86::DetourFunction((PBYTE)&Com_RunFrontend, (PBYTE)addr, jmp ? Detours::X86Option::USE_JUMP : Detours::X86Option::USE_CALL)
	PatchRunFrontend(0x406B32, 0);
	PatchRunFrontend(0x5A6C32, 1);
	PatchRunFrontend(0x679018, 1);

	Detours::X86::DetourFunction((PBYTE)&UI_RunMod_HandleFrontend, (PBYTE)0x45CD7D, Detours::X86Option::USE_CALL);
}
