#include "stdafx.h"

static char* techsetPath = "waw_pimp/techsets/%s%s.techset";
static char* techiquePath = "waw_pimp/techniques/%s.tech";

LONG WINAPI MyUnhandledExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo)
{
	printf("\n\nEXCEPTION DETECTED:\n");
	printf("EIP: 0x%p\n", ExceptionInfo->ContextRecord->Eip);
	printf("EX0: 0x%p\n", ExceptionInfo->ExceptionRecord->ExceptionInformation[0]);
	printf("EX1: 0x%p\n", ExceptionInfo->ExceptionRecord->ExceptionInformation[1]);
	printf("\n\n");

	return EXCEPTION_CONTINUE_SEARCH;
	//return PageGuard_Check(ExceptionInfo);
}

bool g_initted = false;

BOOL cod2rad_Init()
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
	printf("----> Loading custom cod2rad\n");

	AddVectoredExceptionHandler(TRUE, (PVECTORED_EXCEPTION_HANDLER)MyUnhandledExceptionFilter);

	//
	// Threading patches (4 threads -> MAX_THREADS threads)
	//
	PatchThreading();

	//
	// Add Custom Cmd Line Arguments (-HDR Support)
	//
	PatchArguments();

	//
	// Enable Techset / Technique Path Redirection
	//
	PatchMemory(0x0042CA85, (PBYTE)&techiquePath, 4);
	PatchMemory(0x0042CB4C, (PBYTE)&techsetPath, 4);

	//
	// Patch the requested IWI version to match BO1
	//
	PatchMemory(0x00417AA7, (PBYTE)"\xEB", 1);
	PatchMemory(0x00417B41, (PBYTE)"\x30", 1);
	FS_FileOpen = (FS_FileOpen_t)Detours::X86::DetourFunction((PBYTE)0x004034E8, (PBYTE)&FS_ImageRedirect);

	//
	// Patch Xmodel loading functions to support Black Ops
	//
	Detours::X86::DetourFunction((PBYTE)0x00425220, (PBYTE)&hk_XModelLoadConfigFile);

	PatchMemory(0x0042635A, (PBYTE)"\xEB", 1); // Xmodelsurfs version check
	PatchMemory(0x004264AE, (PBYTE)"\xEB", 1); // Xmodelparts version check
	Detours::X86::DetourFunction((PBYTE)0x004476B5, (PBYTE)&mfh_XModelReadSurface); // 4 byte xmodelsurfs file adjustment (MagicNumber)

	//
	// Change the file mode for saving d3dbsp files to "wb+"
	//
	PBYTE p_fmode = (PBYTE)&g_fmode;
	PatchMemory(0x00441B5C, p_fmode, 4);
	PatchMemory(0x00441BDB, p_fmode, 4);

	//
	// Enforce WAW D3DBSP Format on loaded D3DBSP
	//	And BO1 D3DBSP Format on save
	//
	Detours::X86::DetourFunction((PBYTE)0x004413D0, (PBYTE)&hk_FS_FOpenFileRead);
	Detours::X86::DetourFunction((PBYTE)0x00442E97, (PBYTE)&mfh_Com_SaveBsp);

	g_initted = true;
	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Con_Init();
		cod2rad_Init();
		break;
	case DLL_PROCESS_DETACH:
		Con_Restore();
		break;
	}

	return TRUE;
}