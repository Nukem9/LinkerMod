#include "stdafx.h"
#include <stdarg.h>

void (__cdecl * Com_Init)(char *commandLine);

void hk_Com_Init(char *commandLine)
{
	//
	// Fix up the command line because devs removed it
	//
	commandLine = (char *)0x276D0D8;

	Com_Init(commandLine);
}

void Field_Clear(field_t *edit)
{
	((void(__cdecl *)(field_t *))0x0050DB60)(edit);
}

int nullsub(int arg)
{
	return 0;
}

void Com_ToolPrintf(int channel, const char* fmt, ...)
{
	char msg[4096];

	va_list va;
	va_start(va, fmt);
	_vsnprintf_s(msg, 4096, fmt, va);
	va_end(va);

	Com_Printf(channel, "%s", msg);
	printf("%s", msg);
}

void Com_ToolError(int channel, const char* fmt, ...)
{
	char msg[4096];

	va_list va;
	va_start(va, fmt);
	_vsnprintf_s(msg, 4096, fmt, va);
	va_end(va);

	fprintf(stderr, "%s", msg);
	Com_Error(channel, "%s", msg);
}
