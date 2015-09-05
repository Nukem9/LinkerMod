#include "stdafx.h"

int __cdecl XModelReadSurface_MagicFix(BYTE* pos)
{
	float MagicNumber = *(float*)pos;

	int off = 4;
	if (MagicNumber == 27397.0f)
	{
		off += 48;
	}
	else if (MagicNumber != 27398.0)
	{
		off -= 4;
	}

	return off;
}

void* rtn_XModelReadSurface = (void*)0x005351A3;
void __declspec(naked) mfh_XModelReadSurface()
{
	_asm
	{
		mov edx, [ecx]
		add ecx, 4

		push eax
		push ecx
		call XModelReadSurface_MagicFix
		pop ecx
		add ecx, eax
		pop eax
		jmp rtn_test
	}
}