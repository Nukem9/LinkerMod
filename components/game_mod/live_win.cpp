#define _CRT_SECURE_NO_WARNINGS 1
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

//
// Assigns a given value to a buffer and returns the passed offset incremented by
// the number of elements that were consumed or returns -1 if the buffer size was exceeded
//
template<typename ValueType, typename ElemType, size_t N>
static size_t __assign_reserved(ElemType(&buf)[N], size_t offset, ValueType val)
{
	static_assert(sizeof(ValueType) % sizeof(ElemType) == 0, "Invalid ValueType / ElemType combination!");

	size_t result = offset + sizeof(val);

	// If passed an invalid offset, abort
	if (offset < 0)
		return -1;

	// If the new offset would exceed the buffer size, abort
	if (result > N - 1)
		return -1;

	*reinterpret_cast<ValueType*>(buf + offset) = val;
	return result;
}

static bool Session_ResolveModPattern(char(&dst)[_countof(MatchMakingInfo::m_membermod)], const char* fs_game)
{
	//
	// Assign any reserved values
	//
	size_t offset = 0;
	offset = __assign_reserved(dst, offset, (BYTE)GM_NET_VERSION);

	ASSERT(offset >= 0 && offset < _countof(MatchMakingInfo::m_membermod));

	// NOTE: snprintf() returns the number of characters (excluding the null term) that would have been written
	//       if buf_size was large enough, a null term is always added either way
	auto buf_size = _countof(dst) - offset;

	auto n = _snprintf(dst + offset, buf_size, "%s", fs_game);

	if (n + offset > _countof(dst) - 1)
	{
#if DEVELOPMENT
		Com_Error(0,
			"Mod pattern requires %d bytes, exceeds maximum of %d bytes\n"
			"  metadata: (%d bytes)\n"
			"  fs_game:  (%d bytes) '%s'",

			n + offset, _countof(dst) - 1,
			offset,
			n, fs_game);
#else
		Com_Error(0, "fs_game '%s' requires %d bytes when only %d are available", fs_game, n, _countof(dst) - offset - 1);
#endif
		return false;
	}

	return true;
};

void __cdecl Session_Modify_Fix(const int localControllerIndex, void *session, const int flags, const int publicSlots, const int privateSlots)
{
	MatchMakingInfo* g_matchmakingInfo = *(MatchMakingInfo**)0x03879A24;

	Session_ResolveModPattern(g_matchmakingInfo->m_membermod, Dvar_GetString("fs_game"));

	// printf("Setting membermod to %s using net version %d\n", Dvar_GetString("fs_game"), GM_NET_VERSION);

	return Session_Modify(localControllerIndex, session, flags, publicSlots, privateSlots);
}

int SV_UpdateCategory()
{
	return 0;
}

int Live_ClientModMatchesServerMod()
{
	char c_membermod[_countof(MatchMakingInfo::m_membermod)] = "\0";
	Session_ResolveModPattern(c_membermod, Dvar_GetString("fs_game"));

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