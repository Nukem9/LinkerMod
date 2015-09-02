#pragma once

#include <iostream>

#define ASSERT(cond)			if(!(cond)) assert(__FILE__, __LINE__, #cond);
#define ASSERT_MSG(cond, msg)	if(!(cond)) assert(__FILE__, __LINE__, "%s\n\n%s", #cond, msg);

static void assert(const char *File, int Line, const char *Format, ...)
{
	char buffer[4096];
	char message[4096];

	va_list ap;
	va_start(ap, Format);

	_vsnprintf_s(buffer, _TRUNCATE, Format, ap);
	sprintf_s(message, "%s(%d):\n\n%s", File, Line, buffer);

	MessageBoxA(nullptr, message, "ASSERTION", MB_ICONERROR);

	ExitProcess(1);
}