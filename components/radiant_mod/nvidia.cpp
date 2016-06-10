#include "stdafx.h"

bool(__thiscall * sub_NV_10D7D7A0)(void *thisptr, int a2, int a3, int a4);

bool __fastcall hk_sub_NV_10D7D7A0(void *thisptr, void *_EDX, int a2, int a3, int a4)
{
	__try
	{
		return sub_NV_10D7D7A0(thisptr, a2, a3, a4);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		printf("EXCEPTION IN INJECTION DLL\n");
	}

	return true;
}

DWORD WINAPI NvidiaPatchThread(LPVOID lpArg)
{
	UNREFERENCED_PARAMETER(lpArg);

	// Wait for nvidia's injection dll to be loaded
	HMODULE nvidiaDll = GetModuleHandleA("nvda.graphics.interception.dll");

	while (nvidiaDll == nullptr)
	{
		nvidiaDll = GetModuleHandleA("nvda.graphics.interception.dll");
		Sleep(50);
	}

	// Sanity check
	PBYTE functionAddr = (PBYTE)((ULONG_PTR)nvidiaDll + 0xD7D7A0);

	if (functionAddr[0] != 0x55 ||
		functionAddr[1] != 0x8B)
	{
		printf("Invalid nVidia function signature detected (%p)\n", functionAddr);
		return 0;
	}

	// Hook the broken function
	*(PBYTE *)&sub_NV_10D7D7A0 = Detours::X86::DetourFunction(functionAddr, (PBYTE)hk_sub_NV_10D7D7A0);

	printf("nVidia interception DLL hooked (base %p, function %p)\n", nvidiaDll, functionAddr);
	return 0;
}

void PatchNvidiaTools()
{
	HANDLE threadHandle = CreateThread(nullptr, 0, NvidiaPatchThread, nullptr, 0, nullptr);

	ASSERT(threadHandle != nullptr);

	if (threadHandle)
		CloseHandle(threadHandle);
}