#include "str.h"

const char* stristr(const char* s1, const char* s2)
{
	unsigned int s2_len = strlen(s2);

	for (; (*s1 && strlen(s1) >= s2_len); s1++)
	{
		if (_strnicmp(s1, s2, s2_len) == 0)
		{
			return s1;
		}
	}

	return nullptr;
}
