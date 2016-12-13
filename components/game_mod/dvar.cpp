#include "stdafx.h"

dvar_s* sm_quality;
dvar_s* r_noborder;
dvar_s* con_extcon;
dvar_s* con_inputMaxMatchesShown;
dvar_s* cg_drawViewpos;
dvar_s* gm_build_date;

void R_RegisterCustomDvars()
{
	// Borderless window toggle
	r_noborder = Dvar_RegisterInt("r_noborder", 0, 0, 1, 1, "Enable borderless windowed mode");

	// This can go up to 13 - but anything > 11 has artifacts
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
	// For SpotLamp Lightmaps
	//
	PatchMemory(0x0071F976, (PBYTE)&sampleSize, 4);
	PatchMemory(0x0073534F, (PBYTE)&sampleSize, 4);
	PatchMemory(0x00735361, (PBYTE)&smResPower, 1);

	//
	// Patch CreateWindowExA style
	//
	DWORD style = WS_VISIBLE | WS_SYSMENU | WS_CAPTION;

	if (r_noborder->current.enabled)
		style = WS_VISIBLE | WS_POPUP;

	PatchMemory(0x006B7A74, (PBYTE)&style, sizeof(DWORD));// R_CreateWindow
	PatchMemory(0x006B7EC1, (PBYTE)&style, sizeof(DWORD));// R_ResizeWindow
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
void __cdecl CG_RegisterDvars()
{
	CG_RegisterDvars_o();

	con_extcon = Dvar_RegisterInt("con_extcon", 0, 0, 1, 1, "Enable external console window");
	con_inputMaxMatchesShown = Dvar_RegisterInt("con_inputMaxMatchesShown", 24, 1, 64, 1, "Maximum number of suggestions in the console autocomplete preview");
	cg_drawViewpos = Dvar_RegisterInt("cg_drawViewpos", 0, 0, 1, 1, "Draw the current player origin and view angles");
	gm_build_date = Dvar_RegisterString("gm_build_date", __TIMESTAMP__, 0x10 | 0x8, "Compile time for game_mod");

	// Set the max number of suggestions to show in the console autocomplete preview
	PatchMemory(0x00B72F7C, (PBYTE)&con_inputMaxMatchesShown->current.value, 4);

	// Remap "debug_show_viewpos" to "cg_drawViewpos"
	PatchMemory(0x005BF103, (PBYTE)&cg_drawViewpos->name, 4);
}