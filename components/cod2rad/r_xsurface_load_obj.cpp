#include "stdafx.h"

void* rtn_XModelReadSurface = (void*)0x004476BA;
void __declspec(naked) mfh_XModelReadSurface()
{
	_asm
	{
		add edx, 4
		cmp[esp + 2Ch], 1
		jmp rtn_XModelReadSurface
	}
}