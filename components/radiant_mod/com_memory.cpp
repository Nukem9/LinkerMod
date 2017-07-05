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

void *__cdecl Hunk_FindDataForFile(int type, const char *name)
{
	int hash = FS_HashFileName(name, 1024);
	return Hunk_FindDataForFileInternal(type, name, hash);
}

void *__cdecl Hunk_FindDataForFileInternal(int _type, const char *name, int hash)
{
	void* result = NULL;

	_asm
	{
		push _type
		mov edi, name
		mov eax, hash
		mov ebx, 0x004C7A90
		call ebx
		add esp, 4
		mov result, eax
	}

	return result;
}

const char *__cdecl Hunk_SetDataForFile(int _type, const char *name, void *data, void *(__cdecl *alloc)(int))
{
	const char* result = NULL;

	_asm
	{
		push alloc
		push data
		push _type
		mov esi, name
		mov ebx, 0x004C7B10
		add esp, 12
		mov result, eax
	}

	return result;
}
