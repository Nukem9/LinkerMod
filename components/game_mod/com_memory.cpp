#include "stdafx.h"

// /universal/com_memory.cpp:1220
void *Hunk_Alloc(int size, const char *name, int type)
{
	// ASSERT(Sys_IsMainThread() || Sys_IsRenderThread());

	return Hunk_AllocAlign(size, 32, name, type);
}

// /universal/com_memory.cpp:1242
void *Hunk_AllocAlign(int size, int alignment, const char *name, int type)
{
	// ASSERT(Sys_IsMainThread() || Sys_IsRenderThread());
	// ASSERT(s_hunkData);
	ASSERT(!(alignment & (alignment - 1)));
	ASSERT(alignment <= HUNK_MAX_ALIGNMENT);

	return ((void *(__cdecl *)(int, int))0x0043EEB0)(size, alignment);
}

// /universal/com_memory.cpp:1894
void Z_Free(void *ptr, int type)
{
	free(ptr);
}

// /universal/com_memory.cpp:1913
void *Z_Malloc(int size, const char *name, int type)
{
	return malloc(size);
}