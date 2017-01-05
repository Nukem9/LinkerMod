#pragma once

static IDirect3DDevice9*& dx_device = *(IDirect3DDevice9 **)0x03963448;
static int g_disableRendering;
static bool& r_supportCubedMipMaps = *(bool *)0x0396A4F7;

const char *R_ErrorDescription(HRESULT hr);

HWND WINAPI hk_CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL WINAPI hk_AdjustWindowRectEx(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle);
LONG WINAPI hk_SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong);