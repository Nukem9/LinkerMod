#include "stdafx.h"

void* FS_AllocMem(size_t size)
{
	if (size % 4096)
		size += 4096 - (size % 4096);

	void* buf = Z_Malloc(size);
	memset(buf, 0, size);

	return buf;
}
