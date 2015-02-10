#include "stdafx.h"

SRCLINE(175)
void Com_BeginParseSession(const char *filename)
{
	((void(__cdecl *)(const char *))0x004D1560)(filename);
}

SRCLINE(202)
void Com_EndParseSession()
{
	((void(__cdecl *)())0x004D1630)();
}

SRCLINE(224)
void Com_SetSpaceDelimited(int spaceDelimited)
{
	((void(__cdecl *)(int))0x004D1670)(spaceDelimited);
}

SRCLINE(236)
void Com_SetKeepStringQuotes(int keepStringQuotes)
{
	((void(__cdecl *)(int))0x004D1690)(keepStringQuotes);
}

SRCLINE(314)
void Com_SetScriptWarningPrefix(const char *prefix)
{
	((void(__cdecl *)(const char *))0x004D1730)(prefix);
}

SRCLINE(360)
void Com_ScriptError(const char *msg, ...)
{
	char buf[4096];
	va_list va;

	va_start(va, msg);
	_vsnprintf_s(buf, _TRUNCATE, msg, va);

	return ((void(__cdecl *)(const char *, ...))0x004D1800)(buf);
}

SRCLINE(997)
const char *Com_Parse(const char **data_p)
{
	return ((const char *(__cdecl *)(const char **))0x004D1FA0)(data_p);
}

SRCLINE(1056)
int Com_MatchToken(const char **buf_p, const char *match, int warning)
{
	return ((int(__cdecl *)(const char **, const char *, int))0x004D2040)(buf_p, match, warning);
}