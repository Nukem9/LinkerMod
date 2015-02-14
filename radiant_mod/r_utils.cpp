#include "stdafx.h"

SRCLINE(51)
unsigned int R_HashAssetName(const char *name)
{
	int hash = 0;

	for (const char *pos = name; *pos; ++pos)
	{
		ASSERT(*pos < 'A' || *pos > 'Z');
		ASSERT(*pos != '\\\\' || *pos == '/');

		hash = *pos ^ 33 * hash;
	}

	return hash;
}