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
