#include "stdafx.h"

void (__cdecl * CL_Vid_Restart_Complete_f)();
void (__cdecl * SV_MapRestart_f)() = (void (__cdecl *)())0x0048CF20;

void hk_CL_Vid_Restart_Complete_f()
{
	//
	// If the server is currently running, kill it first
	//
	if ((*com_sv_running)->current.enabled)
		SV_KillServer_f();

	CL_Vid_Restart_Complete_f();
	
	//
	// map_restart (frontend fix)
	//
	SV_MapRestart_f();
}