#pragma once
#include <Windows.h>

extern HINSTANCE g_hInstance;

//
// Register the window class for splash windows
// Returns a pointer to the registered class on success, or NULL on failure
//
WNDCLASS* WinSplash_RegisterClass(void);

//
// Unregister & free the window class for splash windows
// Returns non-zero on failure
//
int WinSplash_UnregisterClass(void);

//
// Create a splash window with the given title and splash image
// Returns a handle to the splash window on success, or NULL on failure
//
HWND WinSplash_CreateWindow(LPCTSTR tsTitle, HBITMAP hSplashImage);

//
// Destroy a splash window
// Returns non-zero on failure
//
int WinSplash_DestroyWindow(HWND hWnd);

////////////////////////////////////////////////////////////////////////////

//
// Automatic Splash Screen Generation
// Called by the splash screen creation detour
//
void __cdecl App_CreateSplash(void);

//
// Automatic Splash Screen Cleanup
// Called by the splash screen destruction detour
//
void __cdecl App_DestroySplash(void);
