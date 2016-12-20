#include "stdafx.h"

HMODULE GetLocalDllHandle()
{
	static HMODULE localDll = nullptr;

	if (!localDll)
		localDll = GetModuleHandleA("radiant_mod.dll");

	return localDll;
}

HGLOBAL WINAPI hk_LoadResource(HMODULE hModule, HRSRC hResInfo)
{
	HGLOBAL handle = LoadResource(GetLocalDllHandle(), hResInfo);

	if (!handle)
		handle = LoadResource(hModule, hResInfo);

	return handle;
}

DWORD WINAPI hk_SizeofResource(HMODULE hModule, HRSRC hResInfo)
{
	DWORD resourceSize = SizeofResource(GetLocalDllHandle(), hResInfo);

	if (!resourceSize)
		resourceSize = SizeofResource(hModule, hResInfo);

	return resourceSize;
}

HRSRC WINAPI hk_FindResourceA(HMODULE hModule, LPCSTR lpName, LPCSTR lpType)
{
	HRSRC resource = FindResourceA(GetLocalDllHandle(), lpName, lpType);

	if (!resource)
		resource = FindResourceA(hModule, lpName, lpType);

	return resource;
}

HMENU WINAPI hk_LoadMenuA(HINSTANCE hInstance, LPCSTR lpMenuName)
{
	HMENU menu = LoadMenuA((HINSTANCE)GetLocalDllHandle(), lpMenuName);

	if (!menu)
		menu = LoadMenuA(hInstance, lpMenuName);

	return menu;
}

HCURSOR WINAPI hk_LoadCursorA(HINSTANCE hInstance, LPCSTR lpCursorName)
{
	HCURSOR cursor = LoadCursorA((HINSTANCE)GetLocalDllHandle(), lpCursorName);

	if (!cursor)
		cursor = LoadCursorA(hInstance, lpCursorName);

	return cursor;
}

HACCEL WINAPI hk_LoadAcceleratorsA(HINSTANCE hInstance, LPCSTR lpTableName)
{
	HACCEL accelerators = LoadAcceleratorsA((HINSTANCE)GetLocalDllHandle(), lpTableName);

	if (!accelerators)
		accelerators = LoadAcceleratorsA(hInstance, lpTableName);

	return accelerators;
}

HICON WINAPI hk_LoadIconA(HINSTANCE hInstance, LPCSTR lpIconName)
{
	HICON icon = LoadIconA((HINSTANCE)GetLocalDllHandle(), lpIconName);

	if (!icon)
		icon = LoadIconA(hInstance, lpIconName);

	return icon;
}

HBITMAP WINAPI hk_LoadBitmapA(HINSTANCE hInstance, LPCSTR lpBitmapName)
{
	HBITMAP bitmap = LoadBitmapA((HINSTANCE)GetLocalDllHandle(), lpBitmapName);

	if (!bitmap)
		bitmap = LoadBitmapA(hInstance, lpBitmapName);

	return bitmap;
}