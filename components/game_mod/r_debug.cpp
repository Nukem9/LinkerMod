#include "stdafx.h"

void __cdecl R_AddDebugString(DebugGlobals *debugGlobalsEntry, const float *origin, const float *color, float scale, const char *string)
{
	_asm
	{
		pushad
		push string
		push scale
		push color
		push origin
		mov edi, debugGlobalsEntry
		mov ebx, 0x006FCC50
		call ebx
		add esp, 0x10
		popad
	}
}
