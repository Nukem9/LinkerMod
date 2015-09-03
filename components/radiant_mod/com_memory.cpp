#include "stdafx.h"

SRCLINE(1220)
void *Hunk_Alloc(int size, const char *name, int type)
{
	return Z_Malloc(size);
}

SRCLINE(1894)
void Z_Free(void *ptr)
{
	((void(__cdecl *)(void *))0x004C77B0)(ptr);
}

SRCLINE(1913)
void *Z_Malloc(int size)
{
	return ((void *(__cdecl *)(int))0x004C7820)(size);
}