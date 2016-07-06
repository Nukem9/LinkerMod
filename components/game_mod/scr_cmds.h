#pragma once

enum scriptInstance_t
{
	SCRIPTINSTANCE_SERVER = 0x0,
	SCRIPTINSTANCE_CLIENT = 0x1,
	SCRIPT_INSTANCE_MAX = 0x2,
};

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

//void CGScr_OpenFile();
void GScr_OpenFile();

//void CGScr_CloseFile();
void GScr_CloseFile();

void Scr_PatchFunctions();
