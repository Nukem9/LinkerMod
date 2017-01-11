#pragma once

struct __declspec(align(4)) cmd_function_s
{
	cmd_function_s *next;
	const char *name;
	const char *autoCompleteDir;
	const char *autoCompleteExt;
	void(__cdecl *function)();
	bool consoleAccess;
};

typedef void (__cdecl* Cmd_AddCommandInternal_t)(const char *cmdName, void(__cdecl *function)(), cmd_function_s *allocedCmd);
static Cmd_AddCommandInternal_t Cmd_AddCommandInternal = (Cmd_AddCommandInternal_t)0x00661400;

typedef void (__cdecl* Cmd_RemoveCommand_t)(const char *cmdName);
static Cmd_RemoveCommand_t Cmd_RemoveCommand = (Cmd_RemoveCommand_t)0x005F1A90;

void mfh_Cmd_Exec_f();

extern void(__cdecl * Cmd_ExecuteSingleCommandInternal)(int localClientNum, int controllerIndex, void *item, const char *text, bool restrict);
void hk_Cmd_ExecuteSingleCommandInternal(int localClientNum, int controllerIndex, void *item, const char *text, bool restrict);
void Cbuf_AddText(int localClientNum, const char *text);