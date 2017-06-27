#pragma once
#include "vanilla.h"

#define CMD_MAX_NESTING 8

struct __declspec(align(4)) cmd_function_s
{
	cmd_function_s *next;
	const char *name;
	const char *autoCompleteDir;
	const char *autoCompleteExt;
	void(__cdecl *function)();
	bool consoleAccess;
};

struct CmdArgs
{
	int nesting;
	int localClientNum[8];
	int controllerIndex[8];
	struct itemDef_s *itemDef[8];
	int argshift[8];
	int argc[8];
	const char **argv[8];
	char textPool[8192];
	const char *argvPool[512];
	int usedTextPool[8];
	int totalUsedArgvPool;
	int totalUsedTextPool;
};

static CmdArgs& sv_cmd_args = *(CmdArgs *)0x0243D208;

typedef void (__cdecl* Cmd_AddCommandInternal_t)(const char *cmdName, void(__cdecl *function)(), cmd_function_s *allocedCmd);
static Cmd_AddCommandInternal_t Cmd_AddCommandInternal = (Cmd_AddCommandInternal_t)0x00661400;

VANILLA_FUNC(Cmd_ExecuteSingleCommand, void (__cdecl*)(int localClientNum, int controllerIndex, const char *text), 0x00619D00);

typedef void (__cdecl* Cmd_RemoveCommand_t)(const char *cmdName);
static Cmd_RemoveCommand_t Cmd_RemoveCommand = (Cmd_RemoveCommand_t)0x005F1A90;

void mfh_Cmd_Exec_f();

extern void(__cdecl * Cmd_ExecuteSingleCommandInternal)(int localClientNum, int controllerIndex, void *item, const char *text, bool restrict);
void hk_Cmd_ExecuteSingleCommandInternal(int localClientNum, int controllerIndex, void *item, const char *text, bool restrict);

void Cbuf_AddText(int localClientNum, const char *text);
VANILLA_FUNC(Cbuf_InsertText, void(__cdecl*)(int localClientNum, const char *text), 0x005B3EB0);
VANILLA_FUNC(Cbuf_Execute, void(__cdecl*)(int localClientNum, int controllerIndex), 0x00639930);
VANILLA_FUNC(Cbuf_ExecuteBuffer, void (__cdecl*)(int localClientNum, int controllerIndex, const char *buffer), 0x00480A00);

CmdArgs *__cdecl Cmd_Args();

int __cdecl Cmd_Argc();
const char *__cdecl Cmd_Argv(int argIndex);

static const char *SV_Cmd_Argv(int argIndex)
{
	ASSERT(sv_cmd_args.nesting < CMD_MAX_NESTING);
	ASSERT(argIndex >= 0);

	if (argIndex >= sv_cmd_args.argc[sv_cmd_args.nesting])
		return "";

	return sv_cmd_args.argv[sv_cmd_args.nesting][argIndex];
}

void __cdecl Cmd_Vstr_f();
void __cdecl CL_PlayDemo_f();
