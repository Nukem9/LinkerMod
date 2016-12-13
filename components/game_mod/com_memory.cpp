#include "stdafx.h"

void *Z_Malloc(int size, const char *name, int type)
{
	return malloc(size);
}

void Z_Free(void *ptr, int type)
{
	free(ptr);
}