#pragma once

extern void (__cdecl * CL_Vid_Restart_Complete_f)();

void hk_CL_Vid_Restart_Complete_f();
void CL_CM_LoadMap(const char *mapname);

typedef const char *(__cdecl* CL_GetServerIPAddress_t)();
extern CL_GetServerIPAddress_t CL_GetServerIPAddress_o;

const char *__cdecl CL_GetServerIPAddress();

char *CG_GetLocalClientGlobals(int localClientNum);