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