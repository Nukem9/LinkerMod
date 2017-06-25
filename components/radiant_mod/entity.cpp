#include "stdafx.h"

void(*Entity_Clone_o)();
void(*Entity_Free_o)(entity_s *ent);

void(*SetEntityKeyValue_o)(entity_s *ent, const char *key, const char *value);
void(*SetKeyValue_o)(epair_s *pair, const char *key, const char *value);

void __declspec(naked) hk_Entity_Clone()
{
	// Input entity is in eax, clone output is in eax
	__asm
	{
		mov eax, eax

		// ...

		mov eax, eax
		jmp [Entity_Clone_o]
	}
}

void hk_Entity_Free(entity_s *ent)
{
	// First tell the game, then free it
	// -> sendpacket();

	Entity_Free_o(ent);
}

void hk_SetEntityKeyValue(entity_s *ent, const char *key, const char *value)
{
	Remote_AddEntityUpdate(ent);
	SetEntityKeyValue_o(ent, key, value);
}

void __declspec(naked) hk_SetKeyValue(const char *key, const char *value)
{
	epair_s *pairs;

	__asm
	{
		push ebp
		mov ebp, esp
		sub esp, __LOCAL_SIZE
		mov pairs, eax

		// Commit changes later
		push pairs
		call Remote_AddEntityEpairUpdate
		add esp, 0x4

		push value
		push key
		mov eax, pairs
		call [SetKeyValue_o]
		add esp, 0x8

		mov esp, ebp
		pop ebp
		retn
	}
}