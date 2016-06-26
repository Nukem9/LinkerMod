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

//
// Specialized WinMain detour to allow for SplashScreen generation
// without permanently adding another function to the call stack
//
void* rtn_WinMain = 0x0;
void __declspec(naked) mfh_WinMain()
{
	_asm
	{
		pushad
		call App_CreateSplash
		popad

		jmp rtn_WinMain;
	}
}

BOOL __stdcall SetWindowPlacement_Hidden(HWND hWnd, WINDOWPLACEMENT *lpwndpl)
{
	lpwndpl->showCmd = SW_HIDE;
	return SetWindowPlacement(hWnd, lpwndpl);
}
