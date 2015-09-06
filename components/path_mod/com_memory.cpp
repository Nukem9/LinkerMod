#include "stdafx.h"

SRCLINE(1220)
void *Hunk_Alloc(int size, const char *name, int type)
{
	return Z_Malloc(size);
}

SRCLINE(1894)
void Z_Free(void *ptr)
{
	free(ptr); //Directly using free and malloc appears to be safer
	//((void(__cdecl *)(void *))0x006C48AF)(ptr);
}

SRCLINE(1913)
void *Z_Malloc(int size)
{
	return malloc(size);
	//return ((void *(__cdecl *)(int))0x00554A6D)(size);
}