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

void assert(const char *File, int Line, bool Condition, const char *Format, ...)
{
	char buffer[4096];
	char message[4096];

	va_list ap;
	va_start(ap, Format);

	_vsnprintf_s(buffer, _TRUNCATE, Format, ap);
	sprintf_s(message, "%s(%d):\n\n%s\n\n0x%X", File, Line, buffer, _ReturnAddress());

	MessageBoxA(nullptr, message, "ASSERTION", 0);

	ExitProcess(1);
}

LONG WINAPI MyUnhandledExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo)
{
	printf("\n\nEXCEPTION DETECTED:\n");
	printf("EIP: 0x%p\n", ExceptionInfo->ContextRecord->Eip);
	printf("EX0: 0x%p\n", ExceptionInfo->ExceptionRecord->ExceptionInformation[0]);
	printf("EX1: 0x%p\n", ExceptionInfo->ExceptionRecord->ExceptionInformation[1]);
	printf("\n\n");

	return EXCEPTION_CONTINUE_SEARCH;
}

BOOL LinkerMod_Init()
{
	//
	// Disable STDOUT buffering
	//
	setbuf(stdout, nullptr);

	//
	// Logo to let the user know this loaded
	//
	printf("----> Loading custom linker\n");

	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

	Detours::X86::DetourFunction((PBYTE)0x004C3D30, (PBYTE)assert);
	Detours::X86::DetourFunction((PBYTE)0x00401940, (PBYTE)Com_LoadBsp);
	Detours::X86::DetourFunction((PBYTE)0x004A6D20, (PBYTE)Path_LoadPaths);

	// Fix missing material assertion
	PatchMemory(0x00486C60, (PBYTE)"\x90\x90\x90\x90\x90", 5);
	PatchMemory(0x00486C6A, (PBYTE)"\xEB", 1);

	// Lump %d has a funny size
	//VirtualProtect((LPVOID)0x004017A2, 1, PAGE_EXECUTE_READWRITE, &d);
	//*(BYTE *)0x004017A2 = 0xEB;

	// No primary lights in BSP
	PatchMemory(0x00401E60, (PBYTE)"\xEB", 1);

	//VirtualProtect((LPVOID)0x00401FE7, 1, PAGE_EXECUTE_READWRITE, &d);
	//*(BYTE *)0x00401FE7 = 0xEB;

	//VirtualProtect((LPVOID)0x476B10, 1, PAGE_EXECUTE_READWRITE, &d);
	//*(BYTE *)0x476B10 = 0xC3;

	//VirtualProtect((LPVOID)0x004749BC, 1, PAGE_EXECUTE_READWRITE, &d);
	//*(BYTE *)0x004749BC = 0xEB;

	//VirtualProtect((LPVOID)0x0047BCEB, 1, PAGE_EXECUTE_READWRITE, &d);
	//*(BYTE *)0x0047BCEB = 0xEB;

	//VirtualProtect((LPVOID)0x479870, 1, PAGE_EXECUTE_READWRITE, &d);
	//*(BYTE *)0x479870 = 0xC3;

	//VirtualProtect((LPVOID)0x475B60, 1, PAGE_EXECUTE_READWRITE, &d);
	//*(BYTE *)0x475B60 = 0xC3;

	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		return LinkerMod_Init(); 
	}

	return TRUE;
}