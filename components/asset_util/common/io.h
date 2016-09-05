#pragma once

#include <iostream>
#include <stdarg.h>
#include <WTypes.h>

int Con_Init(void);
int Con_Free(void);

WORD Con_SetAttributes(WORD wAttribs);
WORD Con_GetAttributes(void);

int Con_Print(const char* fmt, ...);
int Con_Error(const char* fmt, ...);
int Con_Warning(const char* fmt, ...);

int Con_Print_v(const char* fmt, ...);
int Con_Error_v(const char* fmt, ...);
int Con_Warning_v(const char* fmt, ...);

int Con_Print_nv(const char* fmt, ...);
int Con_Error_nv(const char* fmt, ...);
int Con_Warning_nv(const char* fmt, ...);
