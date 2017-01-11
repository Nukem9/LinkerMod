#pragma once

static IDirect3DDevice9*& dx_device = *(IDirect3DDevice9 **)0x03963448;
static int g_disableRendering;
static bool& r_supportCubedMipMaps = *(bool *)0x0396A4F7;

typedef void(__cdecl *cmd_function)();
static cmd_function R_Cmd_Screenshot = (cmd_function)0x007244D0;
static cmd_function R_Cmd_ScreenshotJpeg = (cmd_function)0x007244E0;
static cmd_function R_Cmd_ApplyPicmip = (cmd_function)0x007244C0;
static cmd_function R_Cmd_ReloadMaterialTextures = (cmd_function)0x006D4420;
static cmd_function R_Cmd_LoadSun = (cmd_function)0x00735DA0;
static cmd_function R_Cmd_SaveSun = (cmd_function)0x00735E90;
static cmd_function R_StaticModelCacheStats_f = (cmd_function)0x0070C240;
static cmd_function R_StaticModelCacheFlush_f = (cmd_function)0x0070C410;

const char *R_ErrorDescription(HRESULT hr);

HWND WINAPI hk_CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL WINAPI hk_AdjustWindowRectEx(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle);
LONG WINAPI hk_SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong);

void __cdecl R_RegisterCmds();
void __cdecl R_UnregisterCmds();
