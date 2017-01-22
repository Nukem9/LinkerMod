#pragma once

#define LISTSIZE_MAX 32750

void mfh1_Sys_ListFiles();
void mfh2_Sys_ListFiles();
void mfh3_Sys_ListFiles();

extern void* rtn_WinMain;
void mfh_WinMain();

//
// Same as SetWindowPlacement, but overrides lpwndpl->showCmd to SW_HIDE
//
BOOL __stdcall SetWindowPlacement_Hidden(HWND hWnd, WINDOWPLACEMENT *lpwndpl);
static const void* pfn_SetWindowPlacement_Hidden = &SetWindowPlacement_Hidden;

void hk_HandleLaunchArgs(void);
