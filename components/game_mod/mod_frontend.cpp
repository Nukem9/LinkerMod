#include "stdafx.h"

void Com_RunFrontend()
{
	int v0; // esi@2
	char v1; // al@9
	int v2; // esi@9
	int v3; // eax@11
	XZoneInfo zoneInfo; // [sp+408h] [bp-Ch]@6
	// 0x38789A0 - xblive_matchEndingSoon
	Dvar_SetBool((dvar_s *)0x38789A0, false);
	func_41FEE0();
#if 0
	Dvar_SetBool((dvar_s *)0x38789A0, false);
	func_41FEE0();
	dvar_s *mapname = (dvar_s *)0x2899CEC;
	dvar_s *sv_running = (dvar_s *)0x243FD3C;
	char *currentMapname = (char *)Dvar_GetString("mapname");
	if (!mapname
		|| (!sv_running->current.enabled)
		|| I_strnicmp(currentMapname, "menu_", 5) && I_strcmp(currentMapname, "frontend"))
	{
#endif
		/*
		sub_51D1C0(localClientNum, 0.0);
		if (useFastFile->current.enabled)
		{
			zoneInfo.name = 0;
			zoneInfo.allocFlags = 0;
			zoneInfo.freeFlags = 256;
			DB_LoadXAssets(&zoneInfo, 1u, 0);
		}
		
		if (wierd_var)
			memset(wierd_var, 0, 263872 * com_maxclients->current.integer);
		v1 = sub_569EE0();
		Dvar_SetInt(com_desiredMenu, v1);
		*/
		//Dvar_SetBool(systemlink_warning, 0);
		//Dvar_SetBool(onlinegame, 0);
		//Dvar_SetBool(zombiemode, 0);
		//Dvar_SetBool(zombietron, 0);
		//Dvar_SetBool(blackopsmode, 0);
		//Dvar_SetBool(spmode, 0);
		//Dvar_SetBool(arcademode, 0);
		/*
		v2 = sub_5BEE40();
		if (!Com_LocalClients_GetUsedControllerCount())
			sub_5B4CB0(v2, 1);
		v3 = UI_UIContext_GetInfo(v2);
		*/

		Com_Printf(1, "Getting sub_5BEE40 value\nsub_5BEE40() = ");
		v2 = sub_5BEE40();	// why is localClientNum being this huge, I dont get it
							// 1703388
		Com_Printf(1, "%d\nGetting UIContext Info\nUI_UIContext_GetInfo() = ");
		v3 = UI_UIContext_GetInfo(v2);
		Com_Printf(1, "%d\n", v3);
		Com_Printf(1, "Checking for mod_frontend\n");
		if (PartyClient_CheckMapExists("mod_frontend"))
		{
			Com_Printf(1, "mod_frontend exists. Loading mod_frontend...\n");
			Cmd_ExecuteSingleCommandInternal(v2, v3, NULL, "map mod_frontend", false); // doing this until I find a better alternative
			//Cbuf_ExecuteBuffer(v2, v3, "map mod_frontend\n");
		}
		else
		{
			Com_Printf(1, "Loading frontend...\n");
			Cmd_ExecuteSingleCommandInternal(v2, v3, NULL, "map frontend", false); // doing this until I find a better alternative
			//Cbuf_ExecuteBuffer(v2, v3, "map frontend\n");
		}
#if 0
	}
#endif
}