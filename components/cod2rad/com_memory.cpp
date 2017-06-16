#include "stdafx.h"

struct hunkHeader_t
{
	unsigned int magic;
	int size;
	const char *name;
	int dummy;
};
STATIC_ASSERT_SIZE(hunkHeader_t, 16);

struct hunkUsed_t
{
	int permanent;
	int temp;
};
STATIC_ASSERT_SIZE(hunkUsed_t, 8);

VANILLA_VALUE(s_hunkData, char*, 0x174EFFC0);
VANILLA_VALUE(hunk_low, hunkUsed_t, 0x1748F158);

namespace vanilla
{
	auto free = (void(__cdecl*)(void*lpMem))0x00402FD7;
}

void* FS_AllocMem(size_t size)
{
	if (size % 4096)
		size += 4096 - (size % 4096);

	void* buf = Z_Malloc(size);
	memset(buf, 0, size);

	return buf;
}

void* _Hunk_AllocTempMemoryInternal(size_t _size)
{
	static DWORD dwFunc = 0x004219A0;

	void* result = NULL;
	_asm
	{
		mov eax, _size
		call dwFunc
		mov result, eax
	}
	return result;
}

void _Hunk_FreeTempMemory(void *buf)
{
	if (s_hunkData)
	{
		ASSERT(buf);
		hunkHeader_t* hdr = reinterpret_cast<hunkHeader_t*>(buf)-1;
		if (hdr->magic != 0x89537892)
			Com_Error("Hunk_FreeTempMemory: bad magic");
		hdr->magic = 0x89537893;
		ASSERT(hdr == (void *)(s_hunkData + ((hunk_low.temp - hdr->size + 15) & ~15)));
		hunk_low.temp -= hdr->size;
	}
	else
	{
		vanilla::free(buf);
	}
}
