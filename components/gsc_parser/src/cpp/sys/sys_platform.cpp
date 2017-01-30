#include "sys_platform.h"

#ifdef _WIN32

#include <Windows.h>

ssize_t getline(char **lineptr, size_t *n, FILE *stream)
{
	if (lineptr == NULL || stream == NULL || n == NULL)
		return -1;

	//
	// ssize_t is SIGNED
	//
	if (*n > INT_MAX)
		return -1;

	char* bufptr = *lineptr;
	char *p = bufptr;
	ssize_t size = *n;

	int c = fgetc(stream);

	if (c == EOF)
		return -1;

	if (bufptr == NULL)
	{
		size = 128;
		bufptr = (char*)malloc(size);
		if (bufptr == NULL)
			return -1;
	}

	p = bufptr;
	while (c != EOF)
	{
		if ((p - bufptr) > (size - 1))
		{
			size = size + 128;
			bufptr = (char*)realloc(bufptr, size);

			if (bufptr == NULL)
				return -1;
		}
		*p++ = c;

		if (c == '\n')
			break;
		else
			c = fgetc(stream);
	}

	*p++ = '\0';
	*lineptr = bufptr;
	*n = size;

	return p - bufptr - 1;
}

#endif