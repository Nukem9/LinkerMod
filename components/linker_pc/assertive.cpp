#include "stdafx.h"

bool Assert_MyHandler(const char *filename, int line, int type, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	char buffer[4096];
	_vsnprintf_s(buffer, _TRUNCATE, fmt, ap);

	shared_assert(filename, line, buffer);
}