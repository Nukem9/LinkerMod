#include "stdafx.h"

dvar_s* sm_quality = 0x0;
dvar_s* con_extcon = 0x0;

void R_RegisterCustomDvars()
{
	//this can go up to 13 - but anything > 11 has artifacts
	sm_quality = Dvar_RegisterInt("sm_quality", 10, 5, 13, 0x2001, "Exponent for shadow map resolution (2^n) - requires restart - anything over 11 has potential for artifacts");
	
	//
	// Patch ShadowMap SampleSize
	//
	BYTE smResPower = sm_quality->current.integer; //13 is max
	DWORD sampleSize = 1 << smResPower; //default is 1024
	PatchMemory(0x0071F93F, (PBYTE)&sampleSize, 4);
	sampleSize >>= 1;
	smResPower--;
	PatchMemory(0x0071F954, (PBYTE)&sampleSize, 4);

	//
	//For SpotLamp Lightmaps
	//
	PatchMemory(0x0071F976, (PBYTE)&sampleSize, 4);
	PatchMemory(0x0073534F, (PBYTE)&sampleSize, 4);
	PatchMemory(0x00735361, (PBYTE)&smResPower, 1);
}

void* rtn_R_RegisterDvars = (void*)0x006CA283;
void __declspec(naked) mfh_R_RegisterDvars()
{
	_asm
	{
		pushad
		call R_RegisterCustomDvars
		popad
		call Dvar_RegisterInt
		add esp, 0x48
		jmp rtn_R_RegisterDvars
	}
}

CG_RegisterDvars_t CG_RegisterDvars_o = NULL;
void __cdecl CG_RegisterDvars(void)
{
	CG_RegisterDvars_o();

	con_extcon = Dvar_RegisterInt("con_extcon", 0, 0, 1, 0x1, "Enable external console window (requires restart)");

	if (con_extcon->current.enabled)
	{
		// Hijack thread index #9 ("Worker7") to use as the console update thread
		Sys_CreateThread(con_extconsoleThread, 9);
	}
}
