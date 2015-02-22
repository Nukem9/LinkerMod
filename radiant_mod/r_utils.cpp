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

SRCLINE(70)
unsigned int R_HashString(const char *string)
{
	int hash = 0;

	for (const char *pos = string; *pos; pos++)
		hash = (char)(*pos | 0x20) ^ 33 * hash;

	return hash;
}

// R_Init.cpp
SRCLINE(268)
const char *R_ErrorDescription(HRESULT hr)
{
	return ((const char *(__cdecl *)(HRESULT))0x0051A970)(hr);
}