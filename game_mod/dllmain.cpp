#include "stdafx.h"

void PatchMemory(ULONG_PTR Address, PBYTE Data, SIZE_T Size)
{
	DWORD d = 0;
	VirtualProtect((LPVOID)Address, Size, PAGE_EXECUTE_READWRITE, &d);

	for (SIZE_T i = 0; i < Size; i++)
		*(volatile BYTE *)(Address + i) = *Data++;

	VirtualProtect((LPVOID)Address, Size, d, &d);

	FlushInstructionCache(GetCurrentProcess(), (LPVOID)Address, Size);
}

void FixupFunction(ULONG_PTR Address, ULONG_PTR CEGEaxAddress)
{
	DWORD data = (CEGEaxAddress - Address - 5);

	PatchMemory(Address + 0, (PBYTE)"\xE9", 1);
	PatchMemory(Address + 1, (PBYTE)&data, 4);
}

void(__cdecl * Com_Init)(char *commandLine);
void hk_Com_Init(char *commandLine)
{
	//
	// Fix up the command line because devs removed it
	//
	commandLine = (char *)0x276D0D8;

	Com_Init(commandLine);
}

void (__cdecl * Cmd_ExecuteSingleCommandInternal)(int localClientNum, int controllerIndex, void *item, const char *text, bool restrict);
void hk_Cmd_ExecuteSingleCommandInternal(int localClientNum, int controllerIndex, void *item, const char *text, bool restrict)
{
	//
	// Bypass restricted "#dcr#" marker
	//
	if (_strnicmp(text, "#dcr#", 5) == 0)
		text += 5;

	if (strstr(text, "sv_vac"))
		text = "echo \"Command not executed. VAC is disabled.\"";

	Cmd_ExecuteSingleCommandInternal(localClientNum, controllerIndex, item, text, false);
}

BOOL GameMod_Init()
{
	printf("----> Loading game mod\n");
	fflush(stdout);

	//
	// Bypass CEG's code hashes
	//
	Patch_CEG();
	FixupFunction(0x0060CC10, 0x004F20F0);

	//
	// Sys_CheckCrashOrRerun, EAX = TRUE
	//
	PatchMemory(0x004F1930, (PBYTE)"\xB8\x01\x00\x00\x00\xC3", 6);

	//
	// Bypass deletion of 'steam_appid.txt'
	//
	PatchMemory(0x008B41AF, (PBYTE)"\x90\x90", 2);
	PatchMemory(0x008B41A5, (PBYTE)"\x90\x90\x90\x90\x90", 5); // fopen
	PatchMemory(0x008B41B3, (PBYTE)"\x90\x90\x90\x90\x90", 5); // fclose

	//
	// Force sv_vac 0
	//
	PatchMemory(0x005A272C, (PBYTE)"\x00", 1);
	PatchMemory(0x005A272E, (PBYTE)"\x00", 1);
	PatchMemory(0x00655051, (PBYTE)"\x90\x90\x90\x90\x90", 5);

	//
	// "com_introPlayed"
	// "com_startupIntroPlayed"
	//
	PatchMemory(0x0082C0F9, (PBYTE)"\x01", 1);
	PatchMemory(0x0082C111, (PBYTE)"\x01", 1);

	// TEMP CEG
	// 00580460
	PatchMemory(0x00580460, (PBYTE)"\xC3", 1);

	//
	// Always force the cursor to be shown
	//
	//PatchMemory(0x00683C50, (PBYTE)"\xC3", 1); // sub_683C50
	//PatchMemory(0x00867927, (PBYTE)"\xEB", 1); // jz short loc_86793E
	//PatchMemory(0x006EB2CC, (PBYTE)"\xEB", 1); // jz short loc_6EB2DE

	//
	// Command spam removal
	//
	PatchMemory(0x00887575, (PBYTE)"\x90\x90\x90\x90\x90", 5); // "Party_StopParty"
	PatchMemory(0x0043C6DB, (PBYTE)"\x90\x90\x90\x90\x90", 5); // "Clearing migration data\n"
	PatchMemory(0x0051B809, (PBYTE)"\x90\x90\x90\x90\x90", 5); // "Failed to log on.\n"

	//
	// EXE_TOOFEWPLAYERS in a party game
	//
	PatchMemory(0x00618D21, (PBYTE)"\x90\x90\x90\x90\x90", 5);

	//
	// PLATFORM_MISSINGMAP (DLC needs to be purchased)
	// RETURN TRUE
	//
	PatchMemory(0x0041EEC0, (PBYTE)"\xB0\x01\xC3", 3);

	//
	// Allow vid_restart_complete on a listen server
	//
	PatchMemory(0x005D2F59, (PBYTE)"\xEB", 1);

	//
	// Disable command restrictions
	//
	*(uint8_t **)&Cmd_ExecuteSingleCommandInternal = Detours::X86::DetourFunction((PBYTE)0x829AD0, (PBYTE)&hk_Cmd_ExecuteSingleCommandInternal);

	//
	// De-restrict Dvar_ForEachConsoleAccessName and
	// Cmd_ForEachConsoleAccessName
	//
	PatchMemory(0x005F5D4E, (PBYTE)"\x90\x90", 2);
	PatchMemory(0x00610A94, (PBYTE)"\x90\x90", 2);

	//
	// Enable the in-game console
	//
	Detours::X86::DetourFunction((PBYTE)0x00587DC8, (PBYTE)&Con_ToggleConsole);
	Detours::X86::DetourFunction((PBYTE)0x00587633, (PBYTE)&Con_ToggleConsole);
	PatchMemory(0x00587DC8, (PBYTE)"\xE8", 1);
	PatchMemory(0x00587633, (PBYTE)"\xE8", 1);
	PatchMemory(0x0058761C, (PBYTE)"\xEB", 1);

	//
	// Disable error message boxes with developer_script
	//
	PatchMemory(0x005A16F7, (PBYTE)"\x90\x90", 2);
	PatchMemory(0x005A1700, (PBYTE)"\x90\x90", 2);

	//
	// Apply any game hooks
	//
	*(uint8_t **)&Com_Init = Detours::X86::DetourFunction((PBYTE)0x004069C0, (PBYTE)&hk_Com_Init);

	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		return GameMod_Init(); 
	}

	return TRUE;
}