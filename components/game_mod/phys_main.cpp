#include "stdafx.h"

// /physics/phys_main.cpp:1054
void Phys_ObjGetCenterOfMass(int id, float *outPosition)
{
	((void(__cdecl *)(int, float *))0x004D9480)(id, outPosition);
}

// /physics/phys_main.cpp:1286
void Phys_ObjAddForce(int id, const float *worldPos, const float *impulse, const bool relative)
{
	((void(__cdecl *)(int, const float *, const float *, const bool))0x00448900)(id, worldPos, impulse, relative);
}

int Phys_AssertValidPreset(PhysPreset* physPreset)
{
	if (!physPreset || physPreset == (PhysPreset*)0xFFFFFFFF)
	{
		Com_Error(ERR_DROP, "Attempted to spawn physics object without a valid physPreset\n", physPreset);
		return 1;
	}

	return 0;
}

void __declspec(naked) hk_Phys_ObjCreateAxis()
{
	static DWORD dwRetn = 0x0081E037;
	
	_asm
	{
		pushad
		push esi // physPreset
		call Phys_AssertValidPreset
		add esp, 4

		cmp eax, 0
		jnz SKIP

		
		popad
		sub esp, 88h
		push ebx
		jmp dwRetn

	SKIP:
		popad
		retn
	}
}
