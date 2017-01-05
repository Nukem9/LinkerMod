#include "stdafx.h"

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