#include "stdafx.h"

SRCLINE(175)
void Com_BeginParseSession(const char *filename)
{
	((void(__cdecl *)(const char *))0x005608C1)(filename);
}

SRCLINE(202)
void Com_EndParseSession()
{
	((void(__cdecl *)())0x0056094D)();
}

SRCLINE(224)
void Com_SetSpaceDelimited(int spaceDelimited)
{
	((void(__cdecl *)(int))0x0056097D)(spaceDelimited);
}

SRCLINE(236)
void Com_SetKeepStringQuotes(int keepStringQuotes)
{
	((void(__cdecl *)(int))0x0056099E)(keepStringQuotes);
}

SRCLINE(314)
void Com_SetScriptWarningPrefix(const char *prefix)
{
	((void(__cdecl *)(const char *))0x00560A1A)(prefix);
}

SRCLINE(360)
void Com_ScriptError(const char *msg, ...)
{
	char buf[4096];
	va_list va;

	va_start(va, msg);
	_vsnprintf_s(buf, _TRUNCATE, msg, va);
	
	//((void(__cdecl *)(int,const char *, ...))0x00560ABC)(23,buf); //causes crash - replaced with simple printf
	printf("ERROR: %s",buf);
}

SRCLINE(408)
void Com_UngetToken()
{
	char *pi = (char *)0x744770 + 1056 * *(DWORD *)0x748970;

	if (pi[1028])
	{
		Com_ScriptError("UngetToken called twice");
		ASSERT(false);
	}

	pi[1028] = 1;
	*(DWORD *)0x748974 = *(DWORD *)0x748978;
}

SRCLINE(997)
const char *Com_Parse(const char **data_p)
{
	return ((const char *(__cdecl *)(const char **))0x00561003)(data_p);
}

SRCLINE(1056)
int Com_MatchToken(const char **buf_p, const char *match, int warning)
{
	return ((int(__cdecl *)(const char **, const char *, int))0x00561096)(buf_p, match, warning);
}

SRCLINE(1116)
void Com_SkipRestOfLine(const char **data)
{
	((void(__cdecl *)(const char **))0x004D2160)(data);
}

SRCLINE(1196)
float Com_ParseFloat(const char **buf_p)
{
	return (float)atof(Com_Parse(buf_p));
}

SRCLINE(1214)
int Com_ParseInt(const char **buf_p)
{
	return atoi(Com_Parse(buf_p));
}