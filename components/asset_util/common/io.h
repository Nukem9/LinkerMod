#pragma once

#include <iostream>
#include <stdarg.h>
#include <WTypes.h>

//int printf(const char* fmt, ...);

//
// printf for verbose mode
// prints only when verbose mode is active
//
int printf_v(const char* fmt, ...);

//
// printf for verbose mode
// prints only when verbose mode is NOT active
//
int printf_nv(const char* fmt, ...);

/////////////////////////////////////////////////////////////////////

int Con_Init(void);
int Con_Free(void);

WORD Con_SetAttributes(WORD wAttribs);
WORD Con_GetAttributes(void);

int Con_Print(const char* fmt, ...);
int Con_Error(const char* fmt, ...);
int Con_Warning(const char* fmt, ...);
