#include "stdafx.h"

void (__cdecl * CL_Vid_Restart_Complete_f)();

void hk_CL_Vid_Restart_Complete_f()
{
	//
	// If the server is currently running, kill it first
	//
	if ((*com_sv_running)->current.enabled)
		SV_KillServer_f();

	CL_Vid_Restart_Complete_f();
	
	//
	// TODO: Possible map_restart (frontend fix)
	//
}