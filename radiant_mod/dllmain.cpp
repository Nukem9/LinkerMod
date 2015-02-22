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

void FixupFunction(ULONG_PTR Address, ULONG_PTR DestAddress)
{
	DWORD data = (DestAddress - Address - 5);

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

FILE *dumpFile;

void DumpTable_Recurse(CodeConstantSource *Table, int Indent)
{
	//
	// Cached buffer for indents
	//
	char indentBuf[16];
	memset(indentBuf, 0, sizeof(indentBuf));

	for (int i = 0; i < Indent; i++)
		strcat_s(indentBuf, "\t");

	//
	// Enumerate all entries until nullptr is hit
	//
	for (;;)
	{
		CodeConstantSource *entry = Table++;

		if (entry->name == nullptr)
			break;

		//
		// Build the buffer
		//
		char buf[256];

		sprintf_s(buf, "%s{ \"%s\", %d, %x, %d, %d },\n", indentBuf, entry->name, (int)entry->source, entry->subtable, entry->arrayCount, entry->arrayStride);
		fprintf(dumpFile, buf);

		if (entry->subtable)
			DumpTable_Recurse(entry->subtable, Indent + 1);
	}

	fprintf(dumpFile, "\n");
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
	Detours::X86::DetourFunction((PBYTE)0x0052FE70, (PBYTE)&hk_Material_SetPassShaderArguments_DX);

	Detours::X86::DetourFunction((PBYTE)0x005675B0, (PBYTE)&hk_Image_LoadFromFileWithReader);
	FixupFunction(0x004683F0, (ULONG_PTR)&hk_Com_Printf);

	//
	// Hook shader/technique/techset loading functions for PIMP (ShaderWorks)
	//
	Detours::X86::DetourFunction((PBYTE)0x0052F700, (PBYTE)&hk_Material_LoadShader);
	Detours::X86::DetourFunction((PBYTE)0x00530D60, (PBYTE)&Material_LoadTechniqueSet);

	//
	// Hook Xmodel loading functions to support Black Ops
	//
	Detours::X86::DetourFunction((PBYTE)0x004DFAA0, (PBYTE)&hk_XModelLoadConfigFile);

	PatchMemory(0x004E09DB, (PBYTE)"\xEB", 1);// Xmodelsurfs version check
	PatchMemory(0x004DF7F6, (PBYTE)"\xEB", 1);// Xmpdelparts version check
	PatchMemory(0x005351A2, (PBYTE)"\x08", 1);// 4 byte xmodelsurfs file adjustment (MagicNumber)

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

	//
	// Debug INT3 to make sure specific functions are not called
	//
#define DO_NOT_USE(x) PatchMemory((x), (PBYTE)"\xCC", 1)
	/*
	DO_NOT_USE(0x0052EA20);// Material_ParseArgumentSource
	DO_NOT_USE(0x0052E2C0);// Material_ParseSamplerSource
	DO_NOT_USE(0x0052E6E0);// Material_ParseConstantSource
	DO_NOT_USE(0x0052F210);// Material_ParseShaderArguments
	DO_NOT_USE(0x0052E050);// Material_ParseIndexRange
	DO_NOT_USE(0x0052EFB0);// Material_AddShaderArgument
	DO_NOT_USE(0x0052ED40);// Material_SetShaderArguments
	DO_NOT_USE(0x0052EA70);// Material_DefaultArgumentSource
	DO_NOT_USE(0x0052E370);// Material_DefaultSamplerSourceFromTable
	DO_NOT_USE(0x0052E800);// Material_DefaultConstantSourceFromTable
	DO_NOT_USE(0x0052E430);// Material_DefaultSamplerSource
	DO_NOT_USE(0x0052E910);// Material_UnknownShaderworksConstantSource
	//DO_NOT_USE(0x0052FDB0);// Material_GetStreamDestForSemantic
	//DO_NOT_USE(0x0052DA70);// Material_NameForStreamDest
	DO_NOT_USE(0x0052E990);// Material_ElemCountForParamName
	DO_NOT_USE(0x0052D140);// Material_UsingTechnique
	DO_NOT_USE(0x0052F6B0);// Material_CopyTextToDXBuffer
	*/
	MessageBoxA(nullptr, "", "", 0);
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