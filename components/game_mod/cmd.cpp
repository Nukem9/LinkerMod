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

void __cdecl CL_Record_f()
{
	if (Cmd_Argc() > 2)
	{
		Com_Printf(0, "record <demoname>\n");
		return;
	}

	const int localClientNum = 0;
	if (localClientConnectionState != 10)
	{
		Com_Printf(0, "You must be in a level to record.\n");
		return;
	}

	if (clc_demorecording)
	{
		Com_Printf(0, "Already recording.\n");
		return;
	}

	char demoName[64];
	char name[256];
	char nameLiveStream[256];
	if (Cmd_Argc() == 2)
	{
		const char* demo_name = Cmd_Argv(1);
		I_strncpyz(demoName, demo_name, 64);
		sprintf_s(name, 256, "%s.dm_%d", demoName, 7);
		sprintf_s(nameLiveStream, 256, "%s.txt", demoName);
	}
	else
	{
		for (int number = 0; number <= 9999; ++number)
		{
			sprintf_s(demoName, 64, "demo%04i", number);
			sprintf_s(name, 256, "%s.dm_%d", demoName, 7);
			sprintf_s(nameLiveStream, 256, "%s.txt", demoName);
			
			const char* baspath = fs_homepath->current.string;

			char ospath[256];
			FS_BuildOSPath(baspath, "demos", name, ospath);
			if (!FS_OSFPathExists(ospath))
				break;
		}
	}

	Com_Printf(0, "recording to %s.\n", name);
	clc_demofile = FS_SV_FOpenFileWrite(name, "demos");
	if (!clc_demofile)
	{
		Com_PrintError(0, "ERROR: couldn't open.\n");
		return;
	}

	if (!cl_demoLiveStreaming->current.integer
		|| (clc_demoLiveStream = FS_SV_FOpenFileWrite(nameLiveStream, "demos")) != 0)
	{
		clc_demorecording = 1;
		clc_demowaiting = 1;

		//clc_demoRecordStartTime = Sys_Milliseconds();
		//clc_demoUseMemoryBuffer = 0;

		I_strncpyz(clc_demoName, demoName, 64);
		CL_WriteUncompressedDemoInfo(localClientNum);
	}
	else
	{
		Com_PrintError(0, "ERROR: couldn't open to write %s.\n", nameLiveStream);
	}

}

void __cdecl CL_PlayDemo_f()
{
	if (Cmd_Argc() != 2)
	{
		Com_Printf(14, "%s <demoname>\n", Cmd_Argv(0));
		return;
	}

	if (com_sv_running->current.enabled)
	{
		Com_Printf(14, "listen server cannot play a demo.\n");
		return;
	}

	int localClientNum = 0;
	CL_Disconnect(0, 1);

	char name[256];
	char extension[32];
	sprintf_s(extension, 32, ".dm_%d", 7);

	const char* demo = Cmd_Argv(1);
	if (_stricmp(&demo[strlen(demo)] - strlen(extension), extension))
		sprintf_s(name, 256, "%s.dm_%d", demo, 7);
	else
		sprintf_s(name, 256, "%s", demo);

	int demofile = NULL;
	FS_SV_FOpenFileRead(name, "demos", &demofile, NULL);

	if (!demofile)
	{
		Com_Error(ERR_DROP, va("EXE_ERR_NOT_FOUND\x15%s", name));
		return;
	}

	CL_AllocatePerLocalClientMemory(0, 0);

	const char* demoname = Cmd_Argv(1);
	I_strncpyz(clc_demoName, demoname, 64);

	Con_Close(localClientNum);

	localClientConnectionState = 6;

	clc_demofile = demofile;
	clc_demoplaying = 1;

	const char* timedemo = Cmd_Argv(0);
	clc_isTimeDemo = (_stricmp(timedemo, "timedemo") == 0);

	clc_lastClientArchiveIndex = 0;

	const char* servername = Cmd_Argv(1);
	I_strncpyz(cls_servername, servername, 256);
	
	// Disabled because it always throws an error regardless of which csv you use
	//Cbuf_ExecuteBuffer(0, 0, "selectStringTableEntryInDvar mp/didyouknow.csv 0 didyouknow");
	
	if (useFastFile->current.enabled)
		DB_ResetZoneSize(0);

	while (localClientConnectionState >= 6 && localClientConnectionState < 9)
		CL_ReadDemoMessage(localClientNum);

	clc_firstDemoFrameSkipped = 0;
	clc_demoPrevServerTime = 0;
	Dvar_SetIntByName("cl_demoFFSpeed", 0);
}
