#pragma once

#include <iostream>
#include <stdarg.h>
#include <WTypes.h>

int Con_Init(void);
int Con_Free(void);

WORD Con_SetAttributes(WORD wAttribs);
WORD Con_GetAttributes(void);

int Con_Printf(const char* fmt, ...);
int Con_Error(const char* fmt, ...);
int Con_Warning(const char* fmt, ...);

int Con_Printf_v(const char* fmt, ...);
int Con_Error_v(const char* fmt, ...);
int Con_Warning_v(const char* fmt, ...);

int Con_Printf_nv(const char* fmt, ...);
int Con_Error_nv(const char* fmt, ...);
int Con_Warning_nv(const char* fmt, ...);
