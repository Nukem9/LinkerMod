#include "stdafx.h"

dvar_s* gm_build_date = NULL;
dvar_s* gm_version = NULL;

dvar_s* r_d3d9ex = NULL;
dvar_s* r_noborder = NULL;
dvar_s* r_showTess = NULL;
dvar_s* sm_quality = NULL;
dvar_s* con_extcon = NULL;
dvar_s* con_inputMaxMatchesShown = NULL;
dvar_s* cg_drawViewpos = NULL;
dvar_s* scr_useFastFileOnly = NULL;
dvar_s* scr_suppressErrors = NULL;
dvar_s* perk_weapRateEnhanced = NULL;

dvar_s* com_developer_print = NULL;

dvar_s* radiant_live = NULL;
dvar_s* radiant_livePort = NULL;
dvar_s* radiant_liveDebug = NULL;

dvar_s* r_renderTweaks = NULL;
dvar_s* r_renderLit = NULL;
dvar_s* r_renderStandardPostFx = NULL;
dvar_s* r_renderDistortion = NULL;
dvar_s* r_renderEmissive = NULL;
dvar_s* r_renderCorona = NULL;
dvar_s* r_renderSuperflare = NULL;
dvar_s* r_renderSun = NULL;
dvar_s* r_renderReflected = NULL;
dvar_s* r_renderCloakHDR = NULL;

bool com_cfg_readOnly_default = false;
dvar_s* com_cfg_readOnly = NULL;

const char* r_showTessNames[] =
{
	"off",
	"tech",
	"techset",
	"material",
	"vertexshader",
	"pixelshader",
	NULL
};

void R_RegisterCustomDvars()
{
	// DirectX9 extensions
	r_d3d9ex = Dvar_RegisterInt("r_d3d9ex", 0, 0, 1, 1, "Enable DirectX 9Ex improvements");

	// Borderless window toggle
	r_noborder = Dvar_RegisterInt("r_noborder", 0, 0, 1, 1, "Enable borderless windowed mode");

	//this can go up to 13 - but anything > 11 has artifacts
	sm_quality = Dvar_RegisterInt("sm_quality", 10, 5, 13, 0x2000 | DVAR_ARCHIVED, "Exponent for shadow map resolution (2^n) - requires restart - anything over 11 has potential for artifacts");
	
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

	r_showTess = Dvar_RegisterEnum("r_showTess", r_showTessNames, 0, 0x80u, "Show details for each surface");
	com_cfg_readOnly = Dvar_RegisterInt("com_cfg_readOnly", com_cfg_readOnly_default ? 1 : 0, 0, 1, DVAR_AUTOEXEC, "Prevent writing to the config"); // Old flags were 0x2001
	
	static const char* com_dprintf_options[] =
	{
		"default",
		"always",
		"never",
		NULL
	};

	com_developer_print = Dvar_RegisterEnum("developer_print", com_dprintf_options, 0, 0x80, "Modify Com_DPrintf() behavior")
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

	gm_build_date = Dvar_RegisterString("gm_build_date", __TIMESTAMP__, 0x10 | 0x8, "Compile time for game_mod");
	gm_version = Dvar_RegisterString("gm_version", DLL_VersionString(), 0x10 | 0x8, "Game_Mod version");
	
	con_extcon = Dvar_RegisterInt("con_extcon", 0, 0, 1, 1, "Enable external console window");
	con_inputMaxMatchesShown = Dvar_RegisterInt("con_inputMaxMatchesShown", 24, 1, 64, 1, "Maximum number of suggestions in the console autocomplete preview");
	
	cg_drawViewpos = Dvar_RegisterInt("cg_drawViewpos", 0, 0, 1, 1, "Draw the current player origin and view angles");
	scr_useFastFileOnly = Dvar_RegisterInt("scr_useFastFileOnly", 0, 0, 1, 0x2, "Disable the loading of raw script files from the current mod's directory");

	scr_suppressErrors = Dvar_RegisterBool("scr_suppressErrors", false, 0x2, "Suppress fatal script errors");

	perk_weapRateEnhanced = Dvar_RegisterInt("perk_weapRateEnhanced", 0, 0, 1, 0x80, "Double tap will shoot 2x the bullets for every shot");

	radiant_live = Dvar_RegisterInt("radiant_live", 0, 0, 1, 0x0, "Enable live Radiant updates in the game");
	radiant_livePort = Dvar_RegisterInt("radiant_livePort", 3700, 0, 65535, 0x0, "Network port for Radiant");
	radiant_liveDebug = Dvar_RegisterInt("radiant_liveDebug", 0, 0, 1, 0x0, "Debugging prints for Radiant commands");

	// Must be set at launch time to function correctly
	r_renderTweaks = Dvar_RegisterBool("r_renderTweaks", false, 0x2, "Allow renderer tweaks");

	r_renderLit = Dvar_RegisterBool("r_renderLit", true, 0x2, "");
	r_renderStandardPostFx = Dvar_RegisterBool("r_renderStandardPostFx", true, 0x2, "");
	r_renderDistortion = Dvar_RegisterBool("r_renderDistortion", true, 0x2, "");
	r_renderEmissive = Dvar_RegisterBool("r_renderEmissive", true, 0x2, "");
	r_renderCorona = Dvar_RegisterBool("r_renderCorona", true, 0x2, "");
	r_renderSuperflare = Dvar_RegisterBool("r_renderSuperflare", true, 0x2, "");
	r_renderSun = Dvar_RegisterBool("r_renderSun", true, 0x2, "");
	r_renderReflected = Dvar_RegisterBool("r_renderReflected", true, 0x2, "");
	r_renderCloakHDR = Dvar_RegisterBool("r_renderCloakHDR", true, 0x2, "");

	// Set the max number of suggestions to show in the console autocomplete preview
	PatchMemory(0x00B72F7C, (PBYTE)&con_inputMaxMatchesShown->current.value, 4);

	// Remap "debug_show_viewpos" to "cg_drawViewpos"
	PatchMemory(0x005BF103, (PBYTE)&cg_drawViewpos->name, 4);
}

dvar_s *Dvar_SetFromStringByNameFromSource(const char *dvarName, const char *string, DvarSetSource source, unsigned int flags)
{
	return ((dvar_s *(__cdecl *)(const char *, const char *, DvarSetSource, unsigned int))0x00426820)(dvarName, string, source, flags);
}

void Dvar_SetFromStringByName(const char *dvarName, const char *string)
{
	// Do not allow the default FOV to be set. Generally sent with CG_DeployServerCommand.
	if (dvarName && string)
	{
		if (!_stricmp(dvarName, "cg_fov") && !_stricmp(string, "65"))
			return;

		if (!_stricmp(dvarName, "cg_default_fov") && !_stricmp(string, "65"))
			return;
	}

	Dvar_SetFromStringByNameFromSource(dvarName, string, DVAR_SOURCE_INTERNAL, 0);
}

void Dvar_ClearModified(dvar_s *dvar)
{
	dvar->modified = 0;
}
