#pragma once

struct BuiltinFunctionDef
{
	const char *actionString;
	void(__cdecl *actionFunc)();
	int type;
};

VANILLA_FUNC(G_Find, gentity_s *(__cdecl*)(gentity_s *from, int fieldofs, unsigned __int16 match), 0x0054EC60);

VANILLA_FUNC(Scr_GetNumParam, unsigned int(__cdecl*)(scriptInstance_t inst), 0x005C6DA0);
VANILLA_FUNC(Scr_GetInt, int(__cdecl*)(unsigned int index, scriptInstance_t inst), 0x004C1BB0);
VANILLA_FUNC(Scr_GetFloat, float(__cdecl*)(unsigned int index, scriptInstance_t inst), 0x00473380);

VANILLA_FUNC(Scr_AddInt, void(__cdecl*)(int value, scriptInstance_t inst), 0x0045DBB0);

VANILLA_FUNC(Scr_ParamError, void(__cdecl*)(unsigned int index, const char *error, scriptInstance_t inst), 0x0069EDB0)

void CScr_OpenFile();
void GScr_OpenFile();

void CScr_CloseFile();
void GScr_CloseFile();

void CScr_FPrintln();
void GScr_FPrintln();

void CScr_FPrintFields();
void GScr_FPrintFields();

void Scr_PatchFunctions();

void GScr_ChangeLevel(void);
