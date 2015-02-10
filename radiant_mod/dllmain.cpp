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

void strcpy_safe(char *Dest, const char *Src)
{
	size_t srcLen = strlen(Src);
	size_t dstLen = strlen(Dest);

	ASSERT(srcLen <= dstLen);

	DWORD d;
	VirtualProtect(Dest, srcLen, PAGE_EXECUTE_READWRITE, &d);
	strcpy_s(Dest, dstLen, Src);
	VirtualProtect(Dest, srcLen, d, &d);
}

void __declspec(naked) hk_Com_Printf()
{
	__asm
	{
		pushad
		push ebp
		mov ebp, esp

		push [ebp + 0x18 + 0x20]
		push [ebp + 0x14 + 0x20]
		push [ebp + 0x10 + 0x20]
		push [ebp + 0xC + 0x20]
		; push[ebp + 0x8 + 0x20] (int level)
		call printf
		add esp, 0x10

		pop ebp
		popad
		retn
	}
}

bool XModelLoadConfigFile_WAW(const char **pos, int name, int config)
{
	static DWORD dwCall = 0x4DFAA0;

	__asm
	{
		push config
		push name
		mov edi, pos
		call [dwCall]
		add esp, 0x8
	}
}

bool XModelLoadConfigFile_BlackOps(const char **pos, int name, int config)
{
	//
	// Check for World at War's version first
	//
	WORD xmodelVersion = *(WORD *)*pos;

	if (xmodelVersion == 62)
	{
		*pos += 2; // WORD version
		*pos += 3; // Black Ops adjustment

		//
		// Shift the header in order to match WAW's header
		//
		__int16 v3; // cx@1
		bool result; // al@2
		const char *v7; // eax@3
		const char *v8; // edx@3
		int v9; // esi@3
		char v10; // cl@4
		const char *v11; // eax@5
		const char *v12; // edx@5
		int v13; // esi@5
		char v14; // cl@6
		const char *v15; // ecx@7
		int v16; // ebp@7
		float v17; // ST24_4@8
		const char *v18; // ecx@8
		const char *v19; // edx@8
		int v20; // esi@8
		char v21; // al@9
		bool v22; // zf@10
		signed int v23; // [sp+Ch] [bp-8h]@7

			*(BYTE *)(config + 4140) = *(BYTE *)(*pos);
			*pos += 1;

			*(float *)(config + 4112) = *(float *)(*pos);
			*pos += 4;

			*(float *)(config + 4116) = *(float *)(*pos);
			*pos += 4;

			*(float *)(config + 4120) = *(float *)(*pos);
			*pos += 4;

			*(float *)(config + 4124) = *(float *)(*pos);
			*pos += 4;

			*(float *)(config + 4128) = *(float *)(*pos);
			*pos += 4;

			*(float *)(config + 4132) = *(float *)(*pos);
			*pos += 4;

			v7 = *pos;
			v8 = v7;
			v9 = config + 0x102D;
			do
			{
				v10 = *v8;
				*(BYTE *)v9++ = *v8++;
			} while (v10);

			printf("LEN: %d\n", strlen(v7) + 1);
			*pos += strlen(v7) + 1;
			v11 = *pos;
			v12 = *pos;
			v13 = config + 5165;
			do
			{
				v14 = *v12;
				*(BYTE *)v13++ = *v12++;
			} while (v14);

			printf("LEN: %d\n", strlen(v11) + 1);
			*pos += strlen(v11) + 1;

			// BURN ANOTHER STRING
			printf("LEN: %d\n", strlen(*pos) + 1);
			*pos += strlen(*pos) + 1;
			// BURN A FLOAT
			*pos += 4;

			v15 = *pos;
			v16 = config;
			v23 = 4;

			do
			{
				v17 = *(float *)v15;
				v18 = v15 + 4;
				*(float *)(v16 + 1024) = v17;
				v19 = v18;
				v20 = v16;
				do
				{
					v21 = *v19;
					*(BYTE *)v20++ = *v19++;
				} while (v21);
				v16 += 1028;
				v22 = v23-- == 1;
				v15 = &v18[strlen(v18) + 1];
			} while (!v22);
			*(DWORD *)(config + 4136) = *(DWORD *)v15;
			*pos = v15 + 4;
		
			return true;
	}

	return XModelLoadConfigFile_WAW(pos, name, config);
}

bool __declspec(naked) hk_XModelLoadConfigFile()
{
	__asm
	{
		push ebp
		mov ebp, esp

		push [ebp + 0xC]
		push [ebp + 0x8]
		push edi
		call XModelLoadConfigFile_BlackOps
		add esp, 0xC

		pop ebp
		retn
	}
}

BOOL RadiantMod_Init()
{
	printf("----> Loading radiant mod\n");
	fflush(stdout);

	//
	// Create an external console for Radiant
	//
	if (AllocConsole())
	{
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
		freopen("CONIN$", "r", stdin);
	}

	//
	// Hook any needed functions
	//
	Detours::X86::DetourFunction((PBYTE)0x5675B0, (PBYTE)&hk_Image_LoadFromFileWithReader);
	FixupFunction(0x004683F0, (ULONG_PTR)&hk_Com_Printf);
	FixupFunction(0x0052F700, (ULONG_PTR)&hk_Material_LoadShader);

	PatchMemory(0x4DF7F6, (PBYTE)"\xEB", 1);
	PatchMemory(0x4E09DB, (PBYTE)"\xEB", 1);

	//
	// Hook technique/techset loading functions for PIMP (ShaderWorks)
	//
	FixupFunction(0x005190FF, (ULONG_PTR)&Material_LoadTechniqueSet);
	PatchMemory(0x005190FF, (PBYTE)"\xE8", 1);

	//
	// Hook xmodel loading to support Black Ops
	//
	FixupFunction(0x004E036E, (ULONG_PTR)&hk_XModelLoadConfigFile);
	PatchMemory(0x004E036E, (PBYTE)"\xE8", 1);

	//
	// FixRegistryEntries to prevent collision with CoDWAWRadiant - DEV
	//
	strcpy_safe((char *)0x006F8688, "Software\\iw\\CoDBORadiantModTool\\CoDBORadiantModTool");
	strcpy_safe((char *)0x006F0CD0, "Software\\iw\\CoDBORadiantModTool\\IniPrefs");
	strcpy_safe((char *)0x006EC300, "Software\\iw\\CoDBORadiantModTool\\MRU");
	strcpy_safe((char *)0x006F0D08, "iw\\CoDBORadiantModTool");

	//
	// More BO Radiant re-branding of names
	//
	strcpy_safe((char *)0x006F7984, "CoDBORadiantModTool");
	strcpy_safe((char *)0x006ECA30, "You will need to restart CoDBORadiantModTool for the view changes to take place.");
	strcpy_safe((char *)0x006EC5CC, "CoDBORadiantModTool Project files( *.prj )|*.prj||");
	strcpy_safe((char *)0x00749640, "CoDBORadiantModTool Project (*.prj)");

	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		return RadiantMod_Init(); 
	}

	return TRUE;
}