#include "io.h"
#include "../cvar.h"

#define CON_ERROR_COLOR FOREGROUND_RED | FOREGROUND_INTENSITY
#define CON_WARNING_COLOR FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY

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

#define CON_PRINTFUNC_DEF_BODY(DEST) \
	if (!con_h) \
		return -1; \
	va_list ap;	\
	va_start(ap, fmt);\
	int out = vfprintf(DEST, fmt, ap);\
	va_end(ap);

#define CON_PRINTFUNC_VERBOSE if(!g_verbose.ValueBool()) return -1;
#define CON_PRINTFUNC_NONVERBOSE if(g_verbose.ValueBool()) return -1;

int Con_Print(const char* fmt, ...)
{
	CON_PRINTFUNC_DEF_BODY(stdout);
	return out;
}

int Con_Print_v(const char* fmt, ...)
{
	CON_PRINTFUNC_VERBOSE;
	CON_PRINTFUNC_DEF_BODY(stdout);
	return out;
}

int Con_Print_nv(const char* fmt, ...)
{
	CON_PRINTFUNC_NONVERBOSE;
	CON_PRINTFUNC_DEF_BODY(stdout);
	return out;
}

int Con_Error(const char* fmt, ...)
{
	WORD attribs = Con_GetAttributes();
	Con_SetScreenBufferAttributes(con_h, CON_ERROR_COLOR);
	CON_PRINTFUNC_DEF_BODY(stderr);
	Con_SetAttributes(attribs);
	return out;
}

int Con_Error_v(const char* fmt, ...)
{
	CON_PRINTFUNC_VERBOSE;
	WORD attribs = Con_GetAttributes();
	Con_SetScreenBufferAttributes(con_h, CON_ERROR_COLOR);
	CON_PRINTFUNC_DEF_BODY(stderr);
	Con_SetAttributes(attribs);
	return out;
}

int Con_Error_nv(const char* fmt, ...)
{
	CON_PRINTFUNC_NONVERBOSE;
	WORD attribs = Con_GetAttributes();
	Con_SetScreenBufferAttributes(con_h, CON_ERROR_COLOR);
	CON_PRINTFUNC_DEF_BODY(stderr);
	Con_SetAttributes(attribs);
	return out;
}

int Con_Warning(const char* fmt, ...)
{
	WORD attribs = Con_GetAttributes();
	Con_SetScreenBufferAttributes(con_h, CON_WARNING_COLOR);
	CON_PRINTFUNC_DEF_BODY(stdout);
	Con_SetAttributes(attribs);
	return out;
}

int Con_Warning_v(const char* fmt, ...)
{
	CON_PRINTFUNC_VERBOSE;
	WORD attribs = Con_GetAttributes();
	Con_SetScreenBufferAttributes(con_h, CON_WARNING_COLOR);
	CON_PRINTFUNC_DEF_BODY(stdout);
	Con_SetAttributes(attribs);
	return out;
}

int Con_Warning_nv(const char* fmt, ...)
{
	CON_PRINTFUNC_NONVERBOSE;
	WORD attribs = Con_GetAttributes();
	Con_SetScreenBufferAttributes(con_h, CON_WARNING_COLOR);
	CON_PRINTFUNC_DEF_BODY(stdout);
	Con_SetAttributes(attribs);
	return out;
}

#undef CON_PRINTFUNC_VERBOSE
#undef CON_PRINTFUNC_NONVERBOSE
#undef CON_PRINT_FUNCDEF_BODY

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