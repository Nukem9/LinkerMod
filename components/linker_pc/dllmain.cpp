#define G_VERSION 1, 0, 0
#include "stdafx.h"
#include "r_material_load_obj.h"

#define FIND_STATEMAPS 1

LONG WINAPI MyUnhandledExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo)
{
	printf("\n\nEXCEPTION DETECTED:\n");
	printf("EIP: 0x%p\n", ExceptionInfo->ContextRecord->Eip);
	printf("EX0: 0x%p\n", ExceptionInfo->ExceptionRecord->ExceptionInformation[0]);
	printf("EX1: 0x%p\n", ExceptionInfo->ExceptionRecord->ExceptionInformation[1]);
	printf("\n\n");

	return EXCEPTION_CONTINUE_SEARCH;
}

BOOL g_initted = FALSE;
BOOL LinkerMod_Init()
{
	if (g_initted)
		return TRUE;

	//
	// Disable STDOUT buffering
	//
	setvbuf(stdout, nullptr, _IONBF, 0);

	//
	// Logo to let the user know this loaded
	//
	printf("----> Loading custom linker\n");

	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

	Detours::X86::DetourFunction((PBYTE)0x004C3D30, (PBYTE)Assert_MyHandler);
	Detours::X86::DetourFunction((PBYTE)0x00401940, (PBYTE)Com_LoadBsp);
	Detours::X86::DetourFunction((PBYTE)0x004A6D20, (PBYTE)Path_LoadPaths);

	// Fix missing material assertion
	PatchMemory(0x00486C60, (PBYTE)"\x90\x90\x90\x90\x90", 5);
	PatchMemory(0x00486C6A, (PBYTE)"\xEB", 1);

	// No primary lights in BSP
	PatchMemory(0x00401E60, (PBYTE)"\xEB", 1);

	//
	// Fix for early fclose of dependency file
	//
	PatchMemory_WithNOP(0x0041E5B1, 5);
	Detours::X86::DetourFunction((PBYTE)0x0041E689, (PBYTE)&mfh_fcloseDeps);

	//
	// Fix for misleading (incorrect) assertion message
	//
	const char* msg_assertion = "expected 'constant' or 'material', found '%s' instead\n";
	PatchMemory(0x00480D10, (PBYTE)&msg_assertion, 4);

	//
	// Prevent "_acess" spam
	//
	PatchMemory_WithNOP(0x004C6F9B, 5);

	//
	// Prevent dropping pathnodes if a custom ents file is loaded
	//
	Detours::X86::DetourFunction((PBYTE)0x00420493, (PBYTE)&mfh_MapEnts_ParseEntities);

#if FIND_STATEMAPS
	//
	// Custom material loading code for bruteforcing statemaps for missing techniques
	//
	Detours::X86::DetourFunction((PBYTE)0x004853C0, (PBYTE)&Material_LoadRaw);

	PatchCall(0x0047FA0E, (PBYTE)&hk_Material_RegisterStateMap);
#endif

	g_initted = TRUE;
	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		return LinkerMod_Init();
	}

	return TRUE;
}