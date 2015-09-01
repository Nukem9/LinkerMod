#include "stdafx.h"

ULONG_PTR g_Target;
ULONG g_Size;

DWORD tlsSlot;

void PageGuard_Monitor(ULONG_PTR VirtualAddress, ULONG Size)
{
	tlsSlot = TlsAlloc();

	g_Target = VirtualAddress;
	g_Size = Size;

	MEMORY_BASIC_INFORMATION memInfo;
	VirtualQuery((PVOID)g_Target, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));

	DWORD old = 0;
	VirtualProtect((PVOID)g_Target, Size, memInfo.Protect | PAGE_GUARD, &old);
}

LONG PageGuard_Check(PEXCEPTION_POINTERS ExceptionInfo)
{
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION)
	{
		// Store the offset that is trying to be accessed
		TlsSetValue(tlsSlot, (LPVOID)ExceptionInfo->ExceptionRecord->ExceptionInformation[1]);

		ExceptionInfo->ContextRecord->EFlags |= 0x100;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
	{
		ULONG_PTR eip		= (ULONG_PTR)ExceptionInfo->ExceptionRecord->ExceptionAddress;
		ULONG_PTR offset	= (ULONG_PTR)TlsGetValue(tlsSlot);

		// Is the offset in our range?
		if (offset >= g_Target && offset < (g_Target + g_Size))
		{
			if (offset == 0x15999350)
				printf("REMAP(0x%08X, 0x15999350, (ULONG_PTR)&newDword);\n", eip);

			printf("HIT: 0x%X - 0x%X\n", eip, offset);
			fflush(stdout);
		}

		MEMORY_BASIC_INFORMATION memInfo;
		VirtualQuery((PVOID)g_Target, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));

		DWORD old = 0;
		VirtualProtect(memInfo.BaseAddress, memInfo.RegionSize, memInfo.Protect | PAGE_GUARD, &old);
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	printf("wtf: 0x%X\n", ExceptionInfo->ExceptionRecord->ExceptionAddress);
	return EXCEPTION_CONTINUE_SEARCH;
}

DWORD Scan(ULONG_PTR Address)
{
	for (ULONG_PTR i = Address; i < (Address + 5); i++)
	{
		const DWORD val = *(DWORD *)i;

		if (val >= g_Target && val < (g_Target + g_Size))
			return val;
	}

	return 0;
}

void REMAP(ULONG_PTR Address, ULONG_PTR Target, ULONG_PTR New)
{
	for (int i = 0; i < 6; i++)
	{
		if (*(DWORD *)(Address + i) == Target)
		{
			PatchMemory((Address + i), (PBYTE)&New, 4);
			return;
		}
	}

	__debugbreak();
	__assume(0);
}