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

static HANDLE con_h = NULL;
static WORD con_defaultAttributes = NULL;

//
// Get the attributes for the given console
//
static WORD Con_GetScreenBufferAttributes(HANDLE hCon)
{
	CONSOLE_SCREEN_BUFFER_INFO con_info;
	GetConsoleScreenBufferInfo(hCon, &con_info);
	return con_info.wAttributes;
}

WORD Con_GetAttributes(void)
{
	return Con_GetScreenBufferAttributes(con_h);
}

//
// Set the attributes for the given console
//	- returns the new console attributes, or NULL on error
//
static WORD Con_SetScreenBufferAttributes(HANDLE hCon, WORD wAttribs)
{
	return SetConsoleTextAttribute(hCon, wAttribs) ? wAttribs : NULL;
}

WORD Con_SetAttributes(WORD wAttribs)
{
	return Con_SetScreenBufferAttributes(con_h, wAttribs);
}

//
// Reset the console screen buffer attributes to their original values
//	- returns the new attributes
//
static WORD Con_ResetScreenBuferAttributes(void)
{
	return Con_SetScreenBufferAttributes(con_h, con_defaultAttributes);
}

int Con_Print(const char* fmt, ...)
{
	if (!con_h)
		return -1;

	va_list ap;
	va_start(ap, fmt);
	int out = vfprintf(stdout, fmt, ap);
	va_end(ap);
	return out;
}

int Con_Error(const char* fmt, ...)
{
	WORD attribs = Con_GetAttributes();
	Con_SetScreenBufferAttributes(con_h, FOREGROUND_RED | FOREGROUND_INTENSITY);

	if (!con_h)
		return -1;

	va_list ap;
	va_start(ap, fmt);
	int out = vfprintf(stderr, fmt, ap);
	va_end(ap);

	Con_SetAttributes(attribs);
	return out;
}

int Con_Warning(const char* fmt, ...)
{
	WORD attribs = Con_GetAttributes();
	Con_SetScreenBufferAttributes(con_h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	if (!con_h)
		return -1;

	va_list ap;
	va_start(ap, fmt);
	int out = vfprintf(stdout, fmt, ap);
	va_end(ap);

	Con_SetAttributes(attribs);
	return out;
}

int Con_Init(void)
{
	if (!con_h)
	{
		con_h = GetStdHandle(STD_OUTPUT_HANDLE);
		con_defaultAttributes = Con_GetScreenBufferAttributes(con_h);
		//hLogfile = nullptr;
	}

	return 0;
}

int Con_Free(void)
{
	return Con_SetScreenBufferAttributes(con_h, con_defaultAttributes);
}