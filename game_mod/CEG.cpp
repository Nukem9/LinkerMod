#include "stdafx.h"

#ifdef CEG_DEBUG
#define CEG_DEBUG_PRINT Log::Write("Region: 0x%X | Redirected: 0x%X | Return: 0x%X", address, dwNew, _ReturnAddress());
#else
#define CEG_DEBUG_PRINT
#endif

typedef void(__thiscall * sub_5CBF00_t)(void *thisptr, DWORD address, size_t scanSize);
sub_5CBF00_t sub_5CBF00_o;

typedef void(__thiscall * sub_661450_t)(void *thisptr, DWORD address, size_t scanSize);
sub_661450_t sub_661450_o;

typedef void(__thiscall * sub_4E9880_t)(void *thisptr, DWORD address, size_t scanSize);
sub_4E9880_t sub_4E9880_o;

LPVOID g_MemoryBuffer;
ULONG_PTR g_ImageBase;
ULONG_PTR g_ImageEnd;
ULONG_PTR g_ImageCodeSize;

void Patch_CEG()
{
	g_ImageBase		= (ULONG_PTR)GetModuleHandleA(nullptr);
	g_ImageCodeSize = 0x9A2C00 - 0x401000;
	g_ImageEnd		= g_ImageBase + g_ImageCodeSize;

	g_MemoryBuffer = VirtualAlloc(nullptr, g_ImageCodeSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(g_MemoryBuffer, (LPVOID)g_ImageBase, g_ImageCodeSize);

	sub_5CBF00_o = (sub_5CBF00_t)Detours::X86::DetourFunction((PBYTE)0x5CBF00, (PBYTE)&sub_5CBF00);
	sub_661450_o = (sub_661450_t)Detours::X86::DetourFunction((PBYTE)0x661450, (PBYTE)&sub_661450);
	sub_4E9880_o = (sub_4E9880_t)Detours::X86::DetourFunction((PBYTE)0x4E9880, (PBYTE)&sub_4E9880);

	Detours::X86::DetourFunction((PBYTE)0x967760, (PBYTE)&hk_memcpy);
	Detours::X86::DetourFunction((PBYTE)0x8EF04F, (PBYTE)&hk_inline_memcpy);
	Detours::X86::DetourFunction((PBYTE)0x8EF168, (PBYTE)&hk_inline_memcpy2);
}

BOOL DataCompare(const BYTE *pData, const BYTE *bMask, const char *szMask)
{
	while (*szMask)
	{
		if (*szMask == 'x' && *pData != *bMask)
			return FALSE;

		++szMask;
		++bMask;
		++pData;
	}

	return *szMask == 0;
}

DWORD FindPattern(DWORD dwAddress, DWORD dwEnd, PBYTE pbMask, char *szMask)
{
	if (pbMask && szMask)
	{
		for (DWORD i = dwAddress; i < dwEnd; i++)
		{
			if (DataCompare((PBYTE)i, pbMask, szMask))
				return i;
		}
	}

	return 0;
}

void Enumerate(bool(__cdecl * pfnCallback)(DWORD address), PBYTE pbMask, char *szMask)
{
	DWORD dwCurAddress = g_ImageBase;

	while (1)
	{
		dwCurAddress = FindPattern(dwCurAddress, g_ImageEnd, pbMask, szMask);

		if (!dwCurAddress || !pfnCallback(dwCurAddress))
			break;

		dwCurAddress++;
	}
}

void PatchMemory(ULONG_PTR Address, PBYTE Data, SIZE_T Size);
void RunCegTests()
{
	auto ceglog = [](DWORD addr)
	{
		//if (addr == 0x402420)
		//	return true;

		if (addr % 0x10 != 0)
			return false;

		//PatchMemory(addr, (PBYTE)"\xCC", 1);
		//char buf[128];
		//sprintf_s(buf, "0x%x", addr);
		//MessageBoxA(nullptr, "", "", 0);

		//DWORD val = ((DWORD(__cdecl *)())addr)();

		//sprintf_s(buf, "0x%x - 0x%x", addr, val);
		//MessageBoxA(nullptr, buf, "", 0);
		return true;
	};

	Enumerate(ceglog, (PBYTE)"\xE8\x00\x00\x00\x00\xFF\xE0", "x????xx");
	MessageBoxA(nullptr, "", "", 0);
}

DWORD __declspec(noinline) GetNewAddress(DWORD dwOld)
{
	if (dwOld > g_ImageBase && dwOld < g_ImageEnd)
		return ((DWORD)g_MemoryBuffer + (dwOld - g_ImageBase));

	return dwOld;
}

void *hk_memcpy(void *dest, const void *src, size_t size)
{
	PVOID dwNew = (PVOID)GetNewAddress((DWORD)src);

	return memcpy(dest, dwNew, size);
}

void __declspec(naked) hk_inline_memcpy()
{
	static DWORD dwCall = 0x8EE910;
	static DWORD dwJmp = 0x8EF056;

	__asm
	{
		push eax
		push ecx
		push edx

		push esi
		call GetNewAddress
		add esp, 4
		mov esi, eax

		pop edx
		pop ecx
		pop eax

		rep movs dword ptr es:[edi], dword ptr ds:[esi]
		call DWORD PTR ds:[dwCall]
		jmp DWORD PTR ds:[dwJmp]
	}
}

void __declspec(naked) hk_inline_memcpy2()
{
	static DWORD dwJmp = 0x8EF16E;

	__asm
	{
		push eax
		push ecx
		push edx

		push esi
		call GetNewAddress
		add esp, 4
		mov esi, eax

		pop edx
		pop ecx
		pop eax

		rep movs dword ptr es:[edi], dword ptr ds:[esi]
		mov ecx, dword ptr ss:[esp+0x24]
		jmp DWORD PTR ds:[dwJmp]
	}
}

void __fastcall sub_5CBF00(void *thisptr, PVOID _EDX, DWORD address, size_t scanSize)
{
	DWORD dwNew = GetNewAddress(address);

	CEG_DEBUG_PRINT;

	sub_5CBF00_o(thisptr, dwNew, scanSize);
}

void __fastcall sub_661450(void *thisptr, PVOID _EDX, DWORD address, size_t scanSize)
{
	DWORD dwNew = GetNewAddress(address);

	CEG_DEBUG_PRINT;

	sub_661450_o(thisptr, dwNew, scanSize);
}

void __fastcall sub_4E9880(void *thisptr, PVOID _EDX, DWORD address, size_t scanSize)
{
	DWORD dwNew = GetNewAddress(address);

	CEG_DEBUG_PRINT;

	sub_4E9880_o(thisptr, dwNew, scanSize);
}