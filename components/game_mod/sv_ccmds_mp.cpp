#include "stdafx.h"

void SV_KillServer_f()
{
	((void(__cdecl *)())0x0087CF70)();
}

void SV_MapRestart_f()
{
	((void(__cdecl *)())0x0048CF20)();
}

void* SV_ValidateMap_o = 0x0;
void __declspec(naked) SV_ValidateMap()
{
	__asm
	{
		push esi
		mov esi, [esp + 8]
		cmp esi, 1
		jnz DEFAULT
		mov al, 1
		pop esi
		retn

DEFAULT:
		pop esi
		jmp SV_ValidateMap_o
		retn
	}
}

PartyClient_CheckMapExists_t PartyClient_CheckMapExists_o = (PartyClient_CheckMapExists_t)0x0;
bool __cdecl PartyClient_CheckMapExists(const char *map)
{
	if (Com_IsSpecopLevel(map))
	{
		const char* basename = nullptr;

		// Loop through each char until a '_' char is found.
		for (basename = &map[strlen("so_")]; *basename && *basename != '_'; ++basename)
			/* Do nothing */;
		
		if (!*basename)
			Com_PrintError(1, "Bad specop level name\n");

		map = basename + 1;
	}

	return PartyClient_CheckMapExists_o(map);
}

playerState_s *SV_GameClientNum(int num)
{
	return ((playerState_s *(__cdecl *)(int))0x0068FAB0)(num);
}

clientState_s *G_GetClientState(int clientNum)
{
	return ((clientState_s *(__cdecl *)(int))0x005DA470)(clientNum);
}

client_t *SV_GetPlayerByName()
{
	return ((client_t *(__cdecl *)())0x0087C350)();
}

void SV_SetPerk_f()
{
	client_t *cl = SV_GetPlayerByName();

	if (!cl)
	{
		Com_DPrintf(0, "Unknown player\n");
		return;
	}

	// Determine if the perk was valid
	const char *perkName = SV_Cmd_Argv(2);
	unsigned int perkIndex = BG_GetPerkIndexForName(perkName);

	if (perkIndex == PERK_UNKNOWN)
	{
		Com_DPrintf(0, "Unknown perk: %s\n", perkName);
		return;
	}

	// Determine the server index for this player
	int i = 0;

	for (client_t *clIdx = *(client_t **)0x286D01C; i < com_maxclients->current.integer && clIdx != cl; clIdx++)
		++i;

	ASSERT_MSG(i < com_maxclients->current.integer, "i doesn't index com_maxclients->current.integer");

	// Assign perk bit flag to player
	BG_SetPerk(SV_GameClientNum(i)->perks, perkIndex);
	BG_SetPerk(G_GetClientState(i)->perks, perkIndex);
}