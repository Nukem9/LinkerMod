#include "stdafx.h"

// /server_mp/sv_main_mp.cpp:2086
void SV_CheckTimeouts()
{
	((void(__cdecl *)())0x004F5810)();
}

// /server_mp/sv_main_mp.cpp:2264
void SV_PostFrame(bool FrameTimeout)
{
	RadiantRemoteUpdate();								//
	// ((void(__cdecl *)(int))0x00000000)(0);			// Scr_UpdateDebugger
	((void(__cdecl *)())0x004BC500)();					// SV_ProcessPendingSaves
	SV_CheckTimeouts();									//
	((void(__cdecl *)(bool))0x00465890)(FrameTimeout);	// SV_SendClientMessages
	((void(__cdecl *)())0x00601CC0)();					// ????
}

// Allow the server to preload maps (per the default behavior)
// Should always be enabled except when doing ChangeLevel()
int sv_allowMapPreload = 1;

int __cdecl SV_UseMapPreloading(void)
{
	int preload = sv_allowMapPreload;

	// Re-enable preloading for the next check
	if (!preload)
		sv_allowMapPreload = true;

	return preload;
}

void __declspec(naked) mfh_PartyHost_StartMatch(void)
{
	static const DWORD dwRetn_Default = 0x005A3107;
	static const DWORD dwRetn_Skip = 0x005A3122;

	_asm
	{
		push ecx
		push edi
		call SV_UseMapPreloading
		pop edi
		pop ecx

		cmp eax, 0
		jz PRELOAD_SKIP

	// Default preload behavior
		mov ecx, useSvMapPreloading
		cmp byte ptr[ecx + 18h], 0
		jmp dwRetn_Default

	PRELOAD_SKIP:
		jmp dwRetn_Skip
	}
}

void __declspec(naked) mfh_SV_SpawnServer(void)
{
	static const DWORD dwRetn_Default = 0x0050F074;
	static const DWORD dwRetn_Skip = 0x0050F07B;

	_asm
	{
		push ecx
		push edi
		call SV_UseMapPreloading
		pop edi
		pop ecx

		cmp eax, 0
		jz PRELOAD_SKIP

	// Default preload behavior
		mov eax, useSvMapPreloading
		cmp     byte ptr[eax + 18h], 0
		jmp dwRetn_Default

	PRELOAD_SKIP:
		jmp dwRetn_Skip
	}
}
