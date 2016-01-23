#include "io.h"
#include "arg.h"

int printf_v(const char* fmt, ...)
{
	if (!ARG_FLAG_VERBOSE)
		return 0;

	va_list args;
	va_start(args, fmt);
	int result = vprintf(fmt, args);
	va_end(args);

	return result;
}

int printf_nv(const char* fmt, ...)
{
	if (ARG_FLAG_VERBOSE)
		return 0;

	va_list args;
	va_start(args, fmt);
	int result = vprintf(fmt, args);
	va_end(args);

	return result;
}