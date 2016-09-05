#include "io.h"
#include "../cvar.h"

int printf_v(const char* fmt, ...)
{
	if (!g_verbose.ValueBool()) 
		return 0;

	va_list args;
	va_start(args, fmt);
	int result = vprintf(fmt, args);
	va_end(args);

	return result;
}

int printf_nv(const char* fmt, ...)
{
	if (g_verbose.ValueBool())
		return 0;

	va_list args;
	va_start(args, fmt);
	int result = vprintf(fmt, args);
	va_end(args);

	return result;
}