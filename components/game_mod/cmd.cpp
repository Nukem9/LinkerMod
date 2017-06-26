#include "stdafx.h"

char cfg_default_path[] = "/config.cfg";
void* Cmd_ExecFromDisk = (void*)0x008295D0;

typedef void *(__cdecl* Sys_GetValue_t)(int valueIndex);
static Sys_GetValue_t Sys_GetValue = (Sys_GetValue_t)0x0067D4F0;

void* rtn_Cmd_Exec_f = (void*)0x0082A26E;
void __declspec(naked) mfh_Cmd_Exec_f()
{
	_asm
	{
		push ecx
		push esi
		push 0
		push 0
		call Cmd_ExecFromDisk
		cmp al, 0
		je CONFIGFIX
		add esp, 16
		jmp rtn_Cmd_Exec_f

	CONFIGFIX:
		add esp, 8
		pop esi
		pop ecx
		lea esi, cfg_default_path
		call Cmd_ExecFromDisk
		jmp rtn_Cmd_Exec_f
	}
}

void(__cdecl * Cmd_ExecuteSingleCommandInternal)(int localClientNum, int controllerIndex, void *item, const char *text, bool restrict) = NULL;
void hk_Cmd_ExecuteSingleCommandInternal(int localClientNum, int controllerIndex, void *item, const char *text, bool restrict)
{
	// Bypass restricted "#dcr#" marker
	if (_strnicmp(text, "#dcr#", 5) == 0)
		text += 5;

	if (strstr(text, "sv_vac"))
		text = "echo \"Command not executed. VAC is disabled.\"";

	Cmd_ExecuteSingleCommandInternal(localClientNum, controllerIndex, item, text, false);
}

void Cbuf_AddText(int localClientNum, const char *text)
{
	((void(__cdecl *)(int, const char *))0x0049B930)(localClientNum, text);
}

CmdArgs *__cdecl Cmd_Args()
{
	CmdArgs* cmd_args = (CmdArgs*)Sys_GetValue(4);
	return cmd_args;
}

int __cdecl Cmd_Argc()
{
	CmdArgs* cmd_args = Cmd_Args();
	return cmd_args->argc[cmd_args->nesting];
}

const char *__cdecl Cmd_Argv(int argIndex)
{
	CmdArgs* cmd_args = Cmd_Args();
	
	if (argIndex >= cmd_args->argc[cmd_args->nesting])
		return "";
	
	return cmd_args->argv[cmd_args->nesting][argIndex];
}

void __cdecl Cmd_Vstr_f()
{
	if (Cmd_Argc() != 2)
	{
		Com_Printf(0, "vstr <variablename> : execute a variable command\n");
		return;
	}

	const char* dvarName = Cmd_Argv(1);
	dvar_s* dvar = Dvar_FindVar(dvarName);
	
	if (!dvar)
	{
		Com_Printf(0, "%s doesn't exist\n", dvarName);
		return;
	}

	if (dvar->type == DVAR_TYPE_STRING || dvar->type == DVAR_TYPE_ENUM)
	{
		Cbuf_InsertText(0, va("%s\n", dvar->current.string));
	}
	else
	{
		Com_Printf(0, "%s is not a string-based dvar\n", dvar->name);
	}
}