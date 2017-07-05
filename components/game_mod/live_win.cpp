#include "stdafx.h"

const int g_protocolVersion = 3142;

MatchMakingInfo* mminfo = nullptr;

void* Live_PartyHandler = (void*)0x005C2570;
void* Live_JoinSessionInProgressComplete_Continue = (void*)0x00480CB3;
void* Live_JoinSessionInProgressComplete_CancelJoin = (void*)0x00480CC2;
void* Live_AcceptInviteAsyncComplete_Continue = (void*)0x0046B31C;

Session_Modify_t* Session_Modify = (Session_Modify_t*)0x00611930;

bool Com_SessionMode_IsZombiesGame()
{
	return zombiemode->current.enabled;
}

void __cdecl Session_Modify_Fix(const int localControllerIndex, void *session, const int flags, const int publicSlots, const int privateSlots)
{
	MatchMakingInfo* g_matchmakingInfo = *(MatchMakingInfo**)0x03879A24;
	
	g_matchmakingInfo->m_membermod[0] = GM_NET_VERSION;
	sprintf_s(g_matchmakingInfo->m_membermod + 1, 32, "%.*s", 30, Dvar_GetString("fs_game"));
	//printf("Setting membermod to %s using net version %d\n",Dvar_GetString("fs_game"), LM_NET_VERSION);

	return Session_Modify(localControllerIndex, session, flags, publicSlots, privateSlots);
}

int SV_UpdateCategory()
{
	return 0;
}

int Live_ClientModMatchesServerMod()
{
	char c_membermod[32] = "\0";
	c_membermod[0] = GM_NET_VERSION;
	sprintf_s(c_membermod + 1, 32, "%.*s", 30, Dvar_GetString("fs_game"));

	if (strcmp(mminfo->m_membermod, c_membermod) != 0)
	{
		Dvar_SetStringByName("notice_popmenuTitle", UI_SafeTranslateString("@MENU_NOTICE_CAPS"));
		char* msg;
		if (mminfo->m_membermod[0] != c_membermod[0])
		{
			if (mminfo->m_membermod[0] == NULL)
				msg = va("^7This server is not running game_mod.dll.\n Please restart the game without game_mod to join.\n");
			else
				msg = va("^7This server is running an incompatible version of game_mod.dll.\n" \
							"Host is running game_mod with net version: ^3%d^7\n" \
							"You are running game_mod with net version: ^3%d^7", mminfo->m_membermod[0], c_membermod[0]);
		}
		else if (mminfo->m_membermod[1] == 0)
			msg = va("^7This server is not running a mod.\nYou need to unload the current mod before you can join the server.");
		else
			msg = va("^7This server is running the mod ^3%s^7.\nYou need to load the mod before you can join the server.", mminfo->m_membermod + 1);

		Dvar_SetStringByName("notice_popmenuMessage", msg);
		Dvar_SetStringByName("notice_onEscArg", (const char*)0x009DD354);
		Live_UpdateUiPopup(0, "code_notice_popmenu");
		return 0;
	}

	return 1;
}

void __declspec(naked) Live_JoinSessionInProgressComplete_CheckMod()
{
	mminfo = (MatchMakingInfo*)0x038A8FC0;

	if(!Live_ClientModMatchesServerMod())
	{
		_asm
		{
			jmp Live_JoinSessionInProgressComplete_CancelJoin
		}
	}

	_asm
	{
		push 0
		call Live_PartyHandler
		jmp Live_JoinSessionInProgressComplete_Continue
	}
}

void __declspec(naked) Live_AcceptInviteAsyncComplete_CheckMod()
{
	_asm
	{
		mov mminfo, eax
		push edx
	}

	if(!Live_ClientModMatchesServerMod())
	{
		_asm
		{
			pop edx
			add esp, 14h
			pop esi
			add esp, 10h
			retn
		}
	}

	_asm
	{
		mov eax, mminfo
		pop edx
		movq xmm0, qword ptr [eax+0x120]
		jmp Live_AcceptInviteAsyncComplete_Continue
	}
}