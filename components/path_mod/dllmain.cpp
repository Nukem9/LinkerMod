#include "stdafx.h"

//Note: $default.iwi doesnt exist by default in bo1 - it needs to be added manually

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
		; push [ebp + 0x8 + 0x20] (int level)
		call printf
		add esp, 0x10

		pop ebp
		popad
		retn
	}
}

bool g_initted = false;

BOOL PathMod_Init()
{
	if (g_initted)
		return TRUE;

	//
	// Disable STDOUT buffering
	//
	setvbuf(stdout, nullptr, _IONBF, 0);

	
	// Create an external console for Path
	
	if (AllocConsole())
	{
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
		freopen("CONIN$", "r", stdin);
	}
	//
	// Image loading
	//
	Detours::X86::DetourFunction((PBYTE)0x006A2150, (PBYTE)&hk_Image_LoadFromFileWithReader);

	//
	// Reroute Techset / Shader / Technique Directories to use Black Ops Dir Structure in-case of Failure
	//
	FS_ReadFile_o = (FS_ReadFile_t)Detours::X86::DetourFunction((PBYTE)0x0054EC65, (PBYTE)FS_ReadFile);

	//
	// Hook Xmodel loading functions to support Black Ops
	//
	Detours::X86::DetourFunction((PBYTE)0x00578B4B, (PBYTE)&hk_XModelLoadConfigFile);

	PatchMemory(0x00579519, (PBYTE)"\xEB", 1);// Xmodelsurfs version check
	PatchMemory(0x0057892B, (PBYTE)"\xEB", 1);// Xmodelparts version check
	PatchMemory(0x00686F2B, (PBYTE)"\x08", 1);// 4 byte xmodelsurfs file adjustment (MagicNumber)

	//
	// Redirect for default t4 xanims (deprecated)
	//
	//static char* xanim_path_fmt = "xanim/t4/%s";
	//PatchMemory(0x00576A20, (PBYTE)&xanim_path_fmt, 4);

	//
	// Patch to allow for loading of Black Ops xanims (loses support for waw xanims)
	//
	PatchMemory(0x00576A9D, (PBYTE)"\x13", 1);

	//
	// Enforce WAW D3DBSP Format on loaded D3DBSP
	//
	Detours::X86::DetourFunction((PBYTE)0x00521066, (PBYTE)&mfh_Com_LoadBsp);

	g_initted = true;

	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		Con_Init();
		return PathMod_Init(); 
	case DLL_PROCESS_DETACH:
		Con_Restore();
		return TRUE;
	}

	return TRUE;
}