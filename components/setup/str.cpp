#include <Windows.h>
#include "str.h"

const char* stristr(const char* s1, const char* s2)
{
	char *pptr, *sptr, *start;
	unsigned int  slen, plen;

	for (start = (char *)s1,
		pptr = (char *)s2,
		slen = strlen(s1),
		plen = strlen(s2);

		/* while string length not shorter than pattern length */

		slen >= plen;

		start++, slen--)
	{
		/* find start of pattern in string */
		while (toupper(*start) != toupper(*s2))
		{
			start++;
			slen--;

			/* if pattern longer than string */

			if (slen < plen)
				return(NULL);
		}

		sptr = start;
		pptr = (char *)s2;

		while (toupper(*sptr) == toupper(*pptr))
		{
			sptr++;
			pptr++;

			/* if end of pattern then pattern was found */

			if ('\0' == *pptr)
				return (start);
		}
	}
	return(NULL);
}