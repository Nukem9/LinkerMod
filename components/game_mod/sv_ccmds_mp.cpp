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
	if (!I_strncmp("so_", map, strlen("so_")))
	{
		const char* basename = nullptr;
		for (basename = &map[strlen("so_")]; *basename && *basename != '_'; ++basename)
		{
			// Loop through each char until a '_' char is found.
		}
		
		if (*basename == NULL)
		{
			Com_PrintError(1, "Bad specop level name\n");
		}

		map = basename + 1;
	}

	return PartyClient_CheckMapExists_o(map);
}