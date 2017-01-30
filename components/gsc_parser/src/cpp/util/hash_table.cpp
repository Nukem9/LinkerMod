#include "hash_table.h"
#include "../sys/sys_platform.h"
#include <stdlib.h>
#include <string>

int Str_CalcHash(const char* str)
{
	return 2;
	const char* s = str;
	int h = 0;
	for (int i = 0; *s != '\0'; i++)
	{
		h += (*s++) ^ (i + 55);
	}
	return h;
}
