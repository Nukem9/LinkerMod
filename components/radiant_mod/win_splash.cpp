#include "stdafx.h"

HINSTANCE g_hInstance = GetModuleHandle(NULL);

LPCTSTR WC_SPLASH = TEXT("WINSPLASH");
WNDCLASS* wc = NULL;


WNDCLASS* WinSplash_RegisterClass(void)
{
	if (wc)
	{
		return wc;
	}

	wc = new WNDCLASS;
	memset(wc, 0, sizeof(WNDCLASS));

	wc->hInstance = g_hInstance;
	wc->lpfnWndProc = DefWindowProc;
	wc->lpszClassName = WC_SPLASH;
	
	wc->hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc->hCursor = LoadCursor(NULL, IDC_ARROW);
	
	if (!RegisterClass(wc))
	{
		delete wc;
		return NULL;
	}

	return wc;
}

int WinSplash_UnregisterClass(void)
{
	if (!wc)
	{
		return ERROR_CLASS_DOES_NOT_EXIST;
	}

	if (!UnregisterClass(WC_SPLASH, g_hInstance))
	{
		return GetLastError();
	}

	delete wc;
	return 0;
}

POINT WinSplash_CenteredImageOffset(const RECT& targetRect, const SIZE& imgSize)
{
	POINT out;
	out.x = targetRect.left + (targetRect.right - targetRect.left - imgSize.cx) / 2;
	out.y = targetRect.top + (targetRect.bottom - targetRect.top - imgSize.cy) / 2;
	return out;
}

HWND WinSplash_CreateWindow(LPCTSTR tsTitle, HBITMAP hSplashImage)
{
	HWND hWnd = CreateWindowEx(WS_EX_LAYERED, WC_SPLASH, tsTitle, WS_POPUP | WS_VISIBLE, 0, 0, 0, 0, NULL, NULL, g_hInstance, NULL);
	if (!hWnd)
	{
		return hWnd;
	}

	BITMAP bmp;
	GetObject(hSplashImage, sizeof(BITMAP), &bmp);
	SIZE imgSize = { bmp.bmWidth, bmp.bmHeight };

	HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO monInfo = { 0 };
	monInfo.cbSize = sizeof(MONITORINFO);
	if (!GetMonitorInfo(hMon, &monInfo))
	{
		DestroyWindow(hWnd);
		return NULL;
	}

	POINT ptSplashOrigin = WinSplash_CenteredImageOffset(monInfo.rcWork, imgSize);

	HDC sDC = GetDC(NULL);	// Screen device context
	HDC mDC = CreateCompatibleDC(sDC);	// Memory device context
	HBITMAP hOld = (HBITMAP)SelectObject(mDC, hSplashImage); // Original window contents

	POINT ptNULL = { 0 };

#if WINSPLASH_USE_ALPHA
	BLENDFUNCTION blend = { 0 };
	blend.BlendOp = AC_SRC_OVER;
	blend.AlphaFormat = AC_SRC_ALPHA;
	blend.SourceConstantAlpha = 255;

	UpdateLayeredWindow(hWnd, sDC, &ptSplashOrigin, &imgSize, mDC, &ptNULL, RGB(0, 0, 0), &blend, ULW_ALPHA);
#else
	UpdateLayeredWindow(hWnd, sDC, &ptSplashOrigin, &imgSize, mDC, &ptNULL, RGB(0, 0, 0), NULL, ULW_OPAQUE);
#endif

	SelectObject(mDC, hOld);
	DeleteDC(mDC);
	ReleaseDC(NULL, sDC);

	return hWnd;
}

int WinSplash_DestroyWindow(HWND hWnd)
{
	if (!DestroyWindow(hWnd))
	{
		return GetLastError();
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////

HBITMAP hSplashImg = NULL;
HWND hSplashWnd = NULL;

void __cdecl App_CreateSplash()
{
	HBITMAP hSplashImg = (HBITMAP)LoadImage(NULL, APP_SPLASH_PATH, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	if (!WinSplash_RegisterClass())
	{
		return;
	}

	hSplashWnd = WinSplash_CreateWindow(APP_SPLASH_TITLE, hSplashImg);
}

void __cdecl App_DestroySplash()
{
	WinSplash_DestroyWindow(hSplashWnd);
	hSplashWnd = NULL;

	WinSplash_UnregisterClass();
	DeleteObject(hSplashImg);
}
