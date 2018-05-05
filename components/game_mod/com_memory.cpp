#include "stdafx.h"

// /universal/com_memory.cpp:176
void *Z_VirtualReserve(int size, const char *name)
{
	ASSERT(size > 0);

	void *buf = VirtualAlloc(nullptr, size, (size > 0x20000 ? 0 : MEM_TOP_DOWN) | MEM_RESERVE, PAGE_READWRITE);
	ASSERT(buf);

	return buf;
}

// /universal/com_memory.cpp:317
int Z_TryVirtualCommitInternal(void *ptr, int size)
{
	ASSERT(size >= 0);

	return VirtualAlloc(ptr, size, (size > 0x20000 ? 0 : MEM_TOP_DOWN) | MEM_COMMIT, PAGE_READWRITE) != nullptr;
}

// /universal/com_memory.cpp:351
void Z_VirtualFreeInternal(void *ptr)
{
	VirtualFree(ptr, 0, MEM_RELEASE);
}

// /universal/com_memory.cpp:400
void *Z_TryVirtualAllocInternal(int size, const char *name)
{
	// Reserve, then commit (allocate)
	void *ptr = Z_VirtualReserve(size, name);

	if (ptr)
	{
		if (Z_TryVirtualCommitInternal(ptr, size))
			return ptr;

		// If we couldn't commit the memory, free the original reservation
		Z_VirtualFree(ptr, 0);
	}

	return nullptr;
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

// /universal/com_memory.cpp:1726
void *Z_TryVirtualAlloc(int size, const char *name, int type)
{
	void *buf = Z_TryVirtualAllocInternal(size, name);

#if 0
	if (buf)
		track_z_commit((size + 4095) & 0xFFFFF000, type);
#endif

	return buf;
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