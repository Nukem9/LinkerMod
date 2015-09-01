#include "stdafx.h"

void UpdateProgressPrint()
{
	((void(__cdecl *)())0x00429500)();
}

void SetProgress(int a1, int a2)
{
	*(DWORD *)0x1730241C = a1;
	*(DWORD *)0x17302420 = a2;
	UpdateProgressPrint();
}