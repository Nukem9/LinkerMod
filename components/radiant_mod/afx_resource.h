#pragma once

HMODULE GetLocalDllHandle();
HGLOBAL WINAPI hk_LoadResource(HMODULE hModule, HRSRC hResInfo);
DWORD WINAPI hk_SizeofResource(HMODULE hModule, HRSRC hResInfo);
HRSRC WINAPI hk_FindResourceA(HMODULE hModule, LPCSTR lpName, LPCSTR lpType);
HMENU WINAPI hk_LoadMenuA(HINSTANCE hInstance, LPCSTR lpMenuName);
HCURSOR WINAPI hk_LoadCursorA(HINSTANCE hInstance, LPCSTR lpCursorName);
HACCEL WINAPI hk_LoadAcceleratorsA(HINSTANCE hInstance, LPCSTR lpTableName);
HICON WINAPI hk_LoadIconA(HINSTANCE hInstance, LPCSTR lpIconName);
HBITMAP WINAPI hk_LoadBitmapA(HINSTANCE hInstance, LPCSTR lpBitmapName);