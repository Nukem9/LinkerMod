#pragma once
#include "cscr_parser.h"

struct BuiltinFunctionDef
{
	const char *actionString;
	void(__cdecl *actionFunc)();
	int type;
};

typedef unsigned int(__cdecl* Scr_GetNumParam_t)(scriptInstance_t inst);
static Scr_GetNumParam_t Scr_GetNumParam = (Scr_GetNumParam_t)0x005C6DA0;

typedef const char *(__cdecl* Scr_GetString_t)(unsigned int index, scriptInstance_t inst);
static Scr_GetString_t Scr_GetString = (Scr_GetString_t)0x00567CB0;

typedef int (__cdecl* Scr_GetInt_t)(unsigned int index, scriptInstance_t inst);
static Scr_GetInt_t Scr_GetInt = (Scr_GetInt_t)0x004C1BB0;

typedef void (__cdecl* Scr_AddInt_t)(int value, scriptInstance_t inst);
static Scr_AddInt_t Scr_AddInt = (Scr_AddInt_t)0x0045DBB0;

void CScr_OpenFile();
void GScr_OpenFile();

void CScr_CloseFile();
void GScr_CloseFile();

void CScr_FPrintln();
void GScr_FPrintln();

void CScr_FPrintFields();
void GScr_FPrintFields();

void Scr_PatchFunctions();
