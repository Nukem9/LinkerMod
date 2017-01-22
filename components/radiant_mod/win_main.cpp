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

void _cdecl MRU_AddFile(void* pmem, LPCSTR file)
{
	static DWORD dwCall = 0x004A5C70;

	_asm
	{
		push file
		mov esi, pmem
		call dwCall
		add esp, 4
	}
}

void __cdecl MRU_UpdateListMenu(void* pmem, HMENU hMenu)
{
	static DWORD dwCall = 0x004A5DB0;

	_asm
	{
		push hMenu
		mov edi, pmem
		call dwCall
		add esp, 4
	}
}

int SplitArgs(char* cmd_line, int bufferSize, char** buffer)
{
	static DWORD dwCall = 0x004A85A0;
	int argc = 0;

	_asm
	{
		mov esi, buffer
		mov edi, bufferSize
		mov ecx, cmd_line
		call dwCall
		mov argc, eax
	}

	return argc;
}

int ExecuteCommand(char** argv)
{
	static DWORD dwCall = 0x0042D280;
	int result = 0;

	_asm
	{
		mov eax, argv
		call dwCall
		mov result, eax
	}

	return result;
}

void HandleLaunchArgs(const char* cmdline, void* window)
{
	char cmd_line[0x10000];
	strncpy(cmd_line, cmdline, 0x10000);
	cmd_line[0xFFFF] = '\0';

	char* argv[128];
	int argc = SplitArgs(cmd_line, 128, argv);

	_ASSERT(argc >= 1);

	for (int i = 1; i < argc; i++)
	{
		if (*argv[i] == '+')
		{
			int r = ExecuteCommand(&argv[i]);
			i -= (!r) ? 1 : 0;
			continue;
		}

		const char* ext = FS_GetExtensionSubString(argv[1]);
		if (ext == NULL)
			continue;

		if (strcmp(ext, ".map") == 0)
		{
			// Add the map to the MRU registry
			MRU_AddFile(*(void**)0x02857980, argv[1]);
			
			// Actually load the map
			Map_LoadFile_o(argv[1]);

			// Update the MRU menu list
			HMENU menu = GetMenu(*((HWND*)window + 8));
			HMENU subMenu = GetSubMenu(menu, 0);
			MRU_UpdateListMenu(*(void**)0x02857980, subMenu);
		}
	}
}

void __declspec(naked) hk_HandleLaunchArgs(void)
{
	_asm
	{
		pushad
		push edi // window
		push eax // cmd_line
		call HandleLaunchArgs
		add esp, 4
		popad
	}
}

