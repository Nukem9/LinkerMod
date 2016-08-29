#include "stdafx.h"

char* szServerIPAddress = (char*)0x02EFCDA8;

void (__cdecl * CL_Vid_Restart_Complete_f)();

void hk_CL_Vid_Restart_Complete_f()
{
	//
	// If the server is currently running, kill it first
	//
	if ((*com_sv_running)->current.enabled)
		SV_KillServer_f();

	CL_Vid_Restart_Complete_f();
}

void CL_CM_LoadMap(const char *mapname)
{
	((void(__cdecl *)(const char *))0x0056C4B0)(mapname);
}

CL_GetServerIPAddress_t CL_GetServerIPAddress_o = 0x0;

const char *__cdecl CL_GetServerIPAddress()
{
	if (cg_showServerInfo->current.enabled)
	{
		return CL_GetServerIPAddress_o();
	}

	return "";
}
