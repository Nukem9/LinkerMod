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