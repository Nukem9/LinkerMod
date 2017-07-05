#include "stdafx.h"

cmd_function_s& R_Cmd_Screenshot_VAR = *(cmd_function_s*)0x046461F0;
cmd_function_s& R_Cmd_ScreenshotJpeg_VAR = *(cmd_function_s*)0x046461D8;
cmd_function_s& R_ImageList_f_VAR = *(cmd_function_s*)0x046461C0;
cmd_function_s& R_Cmd_ApplyPicmip_VAR = *(cmd_function_s*)0x046461A8;
cmd_function_s& R_Cmd_ReloadMaterialTextures_VAR = *(cmd_function_s*)0x04646190;
cmd_function_s& R_Cmd_LoadSun_VAR = *(cmd_function_s*)0x04646178;
cmd_function_s& R_Cmd_SaveSun_VAR = *(cmd_function_s*)0x04646160;
cmd_function_s& R_StaticModelCacheStats_f_VAR = *(cmd_function_s*)0x04646148;
cmd_function_s& R_StaticModelCacheFlush_f_VAR = *(cmd_function_s*)0x04646130;

cmd_function_s R_FullscreenToggle_f_VAR;
cmd_function_s R_MaterialList_f_VAR;

const char *R_ErrorDescription(HRESULT hr)
{
	return ((const char *(__cdecl *)(HRESULT))0x0098F062)(hr);
}

HWND WINAPI hk_CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	// If this is the main window, set the customized style
	if (lpClassName && strstr(lpClassName, "CoDBlackOps"))
	{
		if (r_noborder->current.enabled)
			dwStyle = WS_VISIBLE | WS_POPUP;
	}

	return CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

BOOL WINAPI hk_AdjustWindowRectEx(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle)
{
	// If windowed mode
	if (dwStyle & (WS_CAPTION | WS_SYSMENU))
	{
		if (r_noborder->current.enabled)
			dwStyle = WS_VISIBLE | WS_POPUP;
	}

	return AdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle);
}

LONG WINAPI hk_SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong)
{
	// If windowed mode
	if (nIndex == GWL_STYLE && (dwNewLong & (WS_CAPTION | WS_SYSMENU)))
	{
		if (r_noborder->current.enabled)
			dwNewLong = WS_VISIBLE | WS_POPUP;
	}

	return SetWindowLongA(hWnd, nIndex, dwNewLong);
}

void R_FullscreenToggle_f()
{
	Cbuf_AddText(0, "toggle r_fullscreen; vid_restart;");
}

//
// Function template for jmping to a specific address if a dvar is enabled
// or doing retn otherwise
//
template<const DWORD _dwJmpTrue, dvar_s*& _dvar>
void __declspec(naked) DvarConditionalCallTemplate(void)
{
	static dvar_s* const & dvar = _dvar;
	static const DWORD dwJmpTrue = _dwJmpTrue;

	_asm
	{
		pushad
		mov ebx, dvar
		mov eax, dword ptr [ebx]
		cmp byte ptr[eax + 0x18], 0
		jz CASE_FALSE

	//CASE_TRUE:
		popad
		jmp dwJmpTrue

	CASE_FALSE:
		popad
		retn
	}
}

#define ADD_DVARCONDITIONALCALL(_ADDRESS, _JMP_TGT_TRUE, _DVAR) PatchCall(_ADDRESS, ForceCastPointer<BYTE>(DvarConditionalCallTemplate<_JMP_TGT_TRUE, _DVAR >))

void R_InitRenderTweaks(void)
{
	if (r_renderTweaks == NULL || !r_renderTweaks->current.enabled)
		return;
	/*
		Add Toggles for various render functions
		A (semi) fix for reflection HDR issues is to disable:
		r_renderLit				// Disable (water, etc.) materials
		r_renderStandardPostFx	// Disable the screenspace reflection buffer
		r_renderDistortion		// Disable the screenspace reflection buffer
	*/
	ADD_DVARCONDITIONALCALL(0x006D27E2, 0x00737ED0, r_renderLit); // This tends to hide water materials
	ADD_DVARCONDITIONALCALL(0x006D2C94, 0x006D1D60, r_renderStandardPostFx);
	ADD_DVARCONDITIONALCALL(0x006D27BA, 0x006D1CF0, r_renderDistortion); // R_ResolveDistortion					
	ADD_DVARCONDITIONALCALL(0x006D29FC, 0x006D1CF0, r_renderDistortion); // R_ResolveDistortion					
	ADD_DVARCONDITIONALCALL(0x006D2A0D, 0x006D1A80, r_renderEmissive);
	ADD_DVARCONDITIONALCALL(0x006D28FC, 0x006E3350, r_renderCorona);
	ADD_DVARCONDITIONALCALL(0x006D290C, 0x00722EF0, r_renderSuperflare);
	ADD_DVARCONDITIONALCALL(0x006D28D6, 0x00723E90, r_renderSun);
	ADD_DVARCONDITIONALCALL(0x006D27D1, 0x006D1C60, r_renderReflected);
	ADD_DVARCONDITIONALCALL(0x006D2C5E, 0x00737720, r_renderCloakHDR);
}

void __cdecl R_RegisterCmds()
{
	Cmd_AddCommandInternal("screenshot", R_Cmd_Screenshot, &R_Cmd_Screenshot_VAR);
	Cmd_AddCommandInternal("screenshotJpeg", R_Cmd_ScreenshotJpeg, &R_Cmd_ScreenshotJpeg_VAR);
	Cmd_AddCommandInternal("imagelist", R_ImageList_f, &R_ImageList_f_VAR); // custom
	Cmd_AddCommandInternal("r_applyPicmip", R_Cmd_ApplyPicmip, &R_Cmd_ApplyPicmip_VAR);
	Cmd_AddCommandInternal("reloadmaterialtextures", R_Cmd_ReloadMaterialTextures, &R_Cmd_ReloadMaterialTextures_VAR);
	Cmd_AddCommandInternal("r_loadsun", R_Cmd_LoadSun, &R_Cmd_LoadSun_VAR);
	Cmd_AddCommandInternal("r_savesun", R_Cmd_SaveSun, &R_Cmd_SaveSun_VAR);
	Cmd_AddCommandInternal("r_smc_stats", R_StaticModelCacheStats_f, &R_StaticModelCacheStats_f_VAR);
	Cmd_AddCommandInternal("r_smc_flush", R_StaticModelCacheFlush_f, &R_StaticModelCacheFlush_f_VAR);

	Cmd_AddCommandInternal("r_fullscreen_toggle", R_FullscreenToggle_f, &R_FullscreenToggle_f_VAR);

	Cmd_AddCommandInternal("gfx_world", R_MaterialList_f, &R_MaterialList_f_VAR); // custom

	R_InitRenderTweaks();
}

void __cdecl R_UnregisterCmds()
{
	Cmd_RemoveCommand("screenshot");
	Cmd_RemoveCommand("screenshotJpeg");
	Cmd_RemoveCommand("imagelist");
	Cmd_RemoveCommand("r_applyPicmip");
	Cmd_RemoveCommand("reloadmaterialtextures");
	Cmd_RemoveCommand("r_loadsun");
	Cmd_RemoveCommand("r_savesun");
	Cmd_RemoveCommand("r_smc_stats");
	Cmd_RemoveCommand("r_smc_flush");

	Cmd_RemoveCommand("r_fullscreen_toggle");

	Cmd_RemoveCommand("gfx_world");
}

struct Font_s *__cdecl R_RegisterFont(const char *name, int imageTrack)
{
	typedef Font_s *(__cdecl *R_RegisterFont_t)(const char *, int);
	R_RegisterFont_t func;

	if (useFastFile->current.enabled)
		func = (R_RegisterFont_t)0x006D30B0; // R_RegisterFont_FastFile;
	else
		func = (R_RegisterFont_t)0x006D3010; // R_RegisterFont_LoadObj;
	return func(name, imageTrack);
}

void __declspec(naked) hk_R_StoreWindowSettings(void)
{
	_asm
	{
		push eax // wndParams
		call R_StoreWindowSettings
		pop eax
		retn
	}
}

void __cdecl R_StoreWindowSettings(GfxWindowParms *wndParms)
{
	ASSERT(r_aspectRatio);

	vidConfig.sceneWidth = wndParms->sceneWidth;
	vidConfig.sceneHeight = wndParms->sceneHeight;
	vidConfig.displayWidth = wndParms->displayWidth;
	vidConfig.displayHeight = wndParms->displayHeight;
	vidConfig.displayFrequency = wndParms->hz;
	vidConfig.isFullscreen = wndParms->fullscreen;

	switch (r_aspectRatio->current.integer)
	{
	case ASPECT_RATIO_AUTO:
	{
		int monitorWidth = vidConfig.displayWidth;
		int monitorHeight = vidConfig.displayHeight;

		if (vidConfig.isFullscreen && dx.adapterNativeIsValid)
		{
			monitorWidth = dx.adapterNativeWidth;
			monitorHeight = dx.adapterNativeHeight;
		}

#if _USE_LEGACY_ASPECT_RATIO_MODE
		// Calculate the height part of the aspect ratio and
		//  set the window's aspect ratio based on that
		int r = (int)(((float)monitorHeight * 16.0f) / (float)monitorWidth);
		if (r == 10)
			vidConfig.aspectRatioWindow = ASPECT_RATIO(16, 10);
		else if (r >= 10)
			vidConfig.aspectRatioWindow = ASPECT_RATIO(4, 3);
		else
			vidConfig.aspectRatioWindow = ASPECT_RATIO(16, 9);
#else
		vidConfig.aspectRatioWindow = (float)monitorWidth / (float)monitorHeight;
#endif

		break;
	}

	case ASPECT_RATIO_STANDARD:
		vidConfig.aspectRatioWindow = ASPECT_RATIO(4, 3);
		break;

	case ASPECT_RATIO_16_9:
		vidConfig.aspectRatioWindow = ASPECT_RATIO(16, 9);
		break;

	case ASPECT_RATIO_16_10:
		vidConfig.aspectRatioWindow = ASPECT_RATIO(16, 10);
		break;

	default:
		ASSERT_MSG_VA(false, "unhandled case, aspectRatio = %i\n", r_aspectRatio->current.integer);
		break;
	}

	ASSERT(com_wideScreen);

	bool isWidescreen = (vidConfig.aspectRatioWindow != ASPECT_RATIO(4, 3));
	Dvar_SetBool(com_wideScreen, isWidescreen);

	vidConfig.aspectRatioScenePixel = (float)vidConfig.sceneHeight * vidConfig.aspectRatioWindow / (float)vidConfig.sceneWidth;
	if (vidConfig.isFullscreen)
		vidConfig.aspectRatioDisplayPixel = ((float)dx.adapterFullscreenHeight * vidConfig.aspectRatioWindow) / (float)dx.adapterFullscreenWidth;
	else
		vidConfig.aspectRatioDisplayPixel = 1.0f;

	vidConfig.isToolMode = r_reflectionProbeGenerate && r_reflectionProbeGenerate->current.enabled;
}