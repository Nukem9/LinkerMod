#include "stdafx.h"

static int list[LISTSIZE_MAX];

int rtn1_Sys_ListFiles = 0x004D706D;
void __declspec(naked) mfh1_Sys_ListFiles()
{
	_asm
	{
		push ecx
		mov ecx, offset list
		mov[ecx + ebx * 4], eax
		pop ecx

		add ebx, 1

		jmp rtn1_Sys_ListFiles
	}
}

int rtn2_Sys_ListFiles = 0x004D709B;
void __declspec(naked) mfh2_Sys_ListFiles()
{
	_asm
	{
		push ecx
		mov ecx, offset list
		mov[ecx + ebx * 4], 0
		pop ecx

		jmp rtn2_Sys_ListFiles
	}
}

int rtn3_Sys_ListFiles = 0x004D70E1;
void __declspec(naked) mfh3_Sys_ListFiles()
{
	_asm
	{
		lea	esi, [list]
		mov edi, eax

		jmp rtn3_Sys_ListFiles
	}
}