#include "stdafx.h"

// /universal/com_memory.cpp:351
void Z_VirtualFreeInternal(void *ptr)
{
	VirtualFree(ptr, 0, MEM_RELEASE);
}

// /universal/com_memory.cpp:1220
void *Hunk_Alloc(int size, const char *name, int type)
{
	ASSERT(Sys_IsMainThread() || Sys_IsRenderThread());

	return Hunk_AllocAlign(size, 32, name, type);
}

// /universal/com_memory.cpp:1242
void *Hunk_AllocAlign(int size, int alignment, const char *name, int type)
{
	ASSERT(Sys_IsMainThread() || Sys_IsRenderThread());
	// ASSERT(s_hunkData);
	ASSERT(!(alignment & (alignment - 1)));
	ASSERT(alignment <= HUNK_MAX_ALIGNMENT);

	return ((void *(__cdecl *)(int, int))0x0043EEB0)(size, alignment);
}

// /universal/com_memory.cpp:1737
void *Z_VirtualAlloc(int size, const char *name, int type)
{
	return ((void *(__cdecl *)(int))0x00696F70)(size);
}

// /universal/com_memory.cpp:1792
void Z_VirtualFree(void *ptr, int type)
{
#if 0
	if (ptr)
	{
		MEMORY_BASIC_INFORMATION info;
		memset(&info, 0, sizeof(MEMORY_BASIC_INFORMATION));

		int freeSize = 0;
		int byteCount = VirtualQuery(ptr, &info, sizeof(MEMORY_BASIC_INFORMATION));

		if (byteCount)
		{
			char *base = info.AllocationBase;

			ASSERT(base == ptr);
			ASSERT(info.BaseAddress == ptr);

			for (char *rangePtr = (char *)ptr;; rangePtr += info.RegionSize)
			{
				byteCount = VirtualQuery(rangePtr, &info, sizeof(MEMORY_BASIC_INFORMATION));

				if (!byteCount)
					break;

				ASSERT(info.BaseAddress == rangePtr);

				if (info.AllocationBase != base)
					break;

				ASSERT(info.RegionSize > 0);

				if (info.State == MEM_COMMIT)
					freeSize += info.RegionSize;
			}

			// if (freeSize)
			//	track_z_decommit(freeSize, type);
		}
	}
#endif

	Z_VirtualFreeInternal(ptr);
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