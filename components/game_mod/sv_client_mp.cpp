#include "stdafx.h"

int& gametime = *(int *)0x0286D014;

int MSG_ReadLong(msg_t *msg)
{
	return ((int (__cdecl *)(msg_t *))0x0061C490)(msg);
}

char *MSG_ReadString(msg_t *msg, char *string, unsigned int maxChars)
{
	ASSERT(string);

	return ((char *(__cdecl *)(msg_t *, char *, unsigned int))0x005B0A80)(msg, string, maxChars);
}

// /server_mp/sv_client_mp.cpp:2303
void SV_DropClient(client_t *drop, const char *reason, bool tellThem, bool writeStats)
{
	((void(__cdecl *)(client_t *, const char *, bool))0x0051F980)(drop, reason, tellThem);
}

// /server_mp/sv_client_mp.cpp:4358
void SV_ExecuteClientCommand(client_t *cl, const char *s, int clientOK, int fromOldServer)
{
	((void (__cdecl *)(client_t *, const char *, int))0x004FB320)(cl, s, clientOK);
}

// /server_mp/sv_client_mp.cpp:4394
int SV_ClientCommand(client_t *cl, msg_t *msg, int fromOldServer)
{
	// ASSERT_MSG(cl - svs_clients) / 544000) >= com_maxclients->current.integer, "cl - svs.clients doesn't index com_maxclients->current.integer");

	char strBuf[1024];
	int seq = MSG_ReadLong(msg);
	const char *s = MSG_ReadString(msg, strBuf, 1024);

	bool clientOk = true;
	bool floodprotect = sv_floodProtect->current.integer && cl->header.state >= 5 && cl->header.netchan.remoteAddress.type != 2;

	// If current command is older than previous, skip
	if (cl->lastClientCommand >= seq)
		return 1;

	if (sv_showCommands->current.enabled)
	 	Com_Printf(15, "clientCommand: %i : %s\n", seq, s);

	// Check if the client skipped/dropped commands on their end (behind server)
	if (seq > cl->lastClientCommand + 1)
	{
		Com_Printf(15, "Client %s lost %i clientCommands\n", cl->name, seq - cl->lastClientCommand + 1);
		SV_DropClient(cl, "EXE_LOSTRELIABLECOMMANDS", true, true);
		return 0;
	}

	// Specific commands are whitelisted from floodprotect
	if (!_strnicmp("team ", s, 5) ||
		!_strnicmp("score", s, 5) ||
		!_strnicmp("mr ", s, 3) ||
		!_strnicmp("act", s, 3))
		floodprotect = false;

	ASSERT(cl->nextReliableCount >= 0);

	// Ignore if too many were sent in under 800ms
	if (fromOldServer || floodprotect && !cl->nextReliableCount && gametime < cl->nextReliableTime)
	{
		clientOk = false;
		Com_DPrintf(15, "Client text ignored for %s: %s\n", cl->name, s);
	}

	if (floodprotect)
	{
		ASSERT(sv_floodProtect->current.integer != 0);

		if (gametime >= cl->nextReliableTime)
		{
			cl->nextReliableCount = sv_floodProtect->current.integer - 1;
		}
		else
		{
			if (cl->nextReliableCount - 1 > 0)
				cl->nextReliableCount -= 1;
			else
				cl->nextReliableCount = 0;
		}

		cl->nextReliableTime = gametime + 800;
	}

	// Parse & execute buffer
	SV_ExecuteClientCommand(cl, s, clientOk, fromOldServer);

	cl->lastClientCommand = seq;
	sprintf_s(cl->lastClientCommandString, "%s", s);
	return 1;
}

void __declspec(naked) hk_SV_ClientCommand()
{
	__asm
	{
		push 0
		push[esp + 8]
		push eax
		call SV_ClientCommand
		add esp, 0xC
		retn
	}
}