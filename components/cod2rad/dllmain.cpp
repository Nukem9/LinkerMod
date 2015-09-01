#include "stdafx.h"

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

BOOL cod2rad_Init()
{
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

	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		return cod2rad_Init();
	}

	return TRUE;
}