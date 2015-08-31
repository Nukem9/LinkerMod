#include "stdafx.h"

MatchMakingInfo* mminfo = nullptr;

void* Live_PartyHandler = (void*)0x005C2570;
void* Live_JoinSessionInProgressComplete_Continue = (void*)0x00480CB3;
void* Live_JoinSessionInProgressComplete_CancelJoin = (void*)0x00480CC2;
void* Live_AcceptInviteAsyncComplete_Continue = (void*)0x0046B31C;

Session_Modify_t* Session_Modify = (Session_Modify_t*)0x00611930;

void __cdecl Session_Modify_Fix(const int localControllerIndex, void *session, const int flags, const int publicSlots, const int privateSlots)
{
	MatchMakingInfo* g_matchmakingInfo = *(MatchMakingInfo**)0x03879A24;
	sprintf_s(g_matchmakingInfo->m_membermod, "%.*s", 32, Dvar_GetString("fs_game"));
	//printf("Settings membermod to %s\n",Dvar_GetString("fs_game"));
	return Session_Modify(localControllerIndex, session, flags, publicSlots, privateSlots);
}

int Live_ClientModMatchesServerMod()
{
	if(strcmp(mminfo->m_membermod, Dvar_GetString("fs_game")) != 0)
	{
		Dvar_SetStringByName("notice_popmenuTitle", UI_SafeTranslateString("@MENU_NOTICE_CAPS"));
		char* msg;
		if(mminfo->m_membermod[0] != NULL)
			msg = va("^7This server is running the mod ^3%s^7.\nYou need to unload the current mod before you can join the server.", mminfo->m_membermod);
		else
			msg = va("^7This server is running the mod ^3%s^7.\nYou need to load the mod before you can join the server.", mminfo->m_membermod);
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