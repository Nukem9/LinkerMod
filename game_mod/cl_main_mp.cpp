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
	// map_restart (frontend fix)
	//
	CL_CM_LoadMap("maps/frontend");
}

void CL_CM_LoadMap(const char *mapname)
{
	((void(__cdecl *)(const char *))0x0056C4B0)(mapname);
}