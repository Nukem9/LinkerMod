#include "stdafx.h"

int level_openScriptIOFileHandles[1];
BYTE* level_openScriptIOFileBuffers[1];

int client_openScriptIOFileHandles[1];
BYTE* client_openScriptIOFileBuffers[1];

BuiltinFunctionDef* client_functions = (BuiltinFunctionDef*)0x00B71DE0;
BuiltinFunctionDef* functions = (BuiltinFunctionDef*)0x00B75EE0;

void Scr_PatchFunctions()
{
	//
	// GSC Patches
	//
	void* ptr = GScr_OpenFile;
	PatchMemory((ULONG_PTR)&functions[369].actionFunc, (PBYTE)&ptr, 4);

	ptr = GScr_CloseFile;
	PatchMemory((ULONG_PTR)&functions[370].actionFunc, (PBYTE)&ptr, 4);

	ptr = GScr_FPrintln;
	PatchMemory((ULONG_PTR)&functions[371].actionFunc, (PBYTE)&ptr, 4);

	ptr = GScr_FPrintFields;
	PatchMemory((ULONG_PTR)&functions[372].actionFunc, (PBYTE)&ptr, 4);

	//
	// CSC Patches
	//
	ptr = CScr_OpenFile;
	PatchMemory((ULONG_PTR)&client_functions[139].actionFunc, (PBYTE)&ptr, 4);

	ptr = CScr_CloseFile;
	PatchMemory((ULONG_PTR)&client_functions[140].actionFunc, (PBYTE)&ptr, 4);

	ptr = CScr_FPrintln;
	PatchMemory((ULONG_PTR)&client_functions[141].actionFunc, (PBYTE)&ptr, 4);

	ptr = CScr_FPrintFields;
	PatchMemory((ULONG_PTR)&client_functions[142].actionFunc, (PBYTE)&ptr, 4);
}

void CScr_OpenFile()
{
	if (!developer->current.enabled || !developer_script->current.enabled)
	{
		Scr_AddInt(-1, SCRIPTINSTANCE_CLIENT);
	}

	int* f = 0;
	if (Scr_GetNumParam(SCRIPTINSTANCE_CLIENT) > 1)
	{
		const char* filename = Scr_GetString(0, SCRIPTINSTANCE_CLIENT);
		const char* mode = Scr_GetString(1, SCRIPTINSTANCE_CLIENT);

		int filenum = 0;
		for (filenum = 0; filenum < 1; ++filenum)
		{
			if (!client_openScriptIOFileHandles[filenum])
			{
				f = &client_openScriptIOFileHandles[filenum];
				break;
			}
		}

		if (f)
		{
			char *fullpathname = va("%s/%s", "scriptdata", filename);

			//
			// Prevent Scripts from Opening Files Outside of the ScriptData Directory
			//
			char* possibleRelativePath = strstr((char*)filename, "..");
			if (possibleRelativePath == filename && strlen(possibleRelativePath) >= 3)
			{
				if (possibleRelativePath[2] == '/' || possibleRelativePath[2] == '\\')
				{
					Com_Printf(24, "OpenFile failed.  Relative paths are not allowed!\n");
					Scr_AddInt(-1, SCRIPTINSTANCE_CLIENT);
				}
			}

			if (!strcmp(mode, "read"))
			{
				int tempFile = NULL;
				int filesize = FS_FOpenFileByMode(fullpathname, &tempFile, FS_READ);

				if (filesize >= 0)
				{
					client_openScriptIOFileBuffers[filenum] = (BYTE*)malloc(filesize + 1);
					FS_Read(client_openScriptIOFileBuffers[filenum], filesize, tempFile);
					FS_FCloseFile(tempFile);
					client_openScriptIOFileBuffers[filenum][filesize] = 0;

					Com_BeginParseSession(filename);
					//Com_SetCSV(1);

					//dword_3E53D10[5 * filenum] = 0;

					Scr_AddInt(filenum, SCRIPTINSTANCE_CLIENT);
				}
				else
				{
					Scr_AddInt(-1, SCRIPTINSTANCE_CLIENT);
				}
			}
			else if (!strcmp(mode, "write"))
			{
				*f = FS_FOpenTextFileWrite(fullpathname);
				if (*f)
					Scr_AddInt(filenum, SCRIPTINSTANCE_CLIENT);
				else
					Scr_AddInt(-1, SCRIPTINSTANCE_CLIENT);
			}
			else if (!strcmp(mode, "append"))
			{
				if (FS_FOpenFileByMode(fullpathname, f, FS_APPEND) >= 0)
					Scr_AddInt(filenum, SCRIPTINSTANCE_CLIENT);
				else
					Scr_AddInt(-1, SCRIPTINSTANCE_CLIENT);
			}
			else
			{
				Com_Printf(24, "Valid openfile modes are 'write', 'read', and 'append'\n");
				Scr_AddInt(-1, SCRIPTINSTANCE_CLIENT);
			}
		}
		else
		{
			Com_Printf(24, "OpenFile failed.  %i files already open\n", 1);
			Scr_AddInt(-1, SCRIPTINSTANCE_CLIENT);
		}
	}
}

void GScr_OpenFile()
{
	if (!developer->current.enabled || !developer_script->current.enabled)
	{
		Scr_AddInt(-1, SCRIPTINSTANCE_SERVER);
	}

	int* f = 0;
	if (Scr_GetNumParam(SCRIPTINSTANCE_SERVER) > 1)
	{
		const char* filename = Scr_GetString(0, SCRIPTINSTANCE_SERVER);
		const char* mode = Scr_GetString(1, SCRIPTINSTANCE_SERVER);

		int filenum = 0;
		for (filenum = 0; filenum < 1; ++filenum)
		{
			if (!level_openScriptIOFileHandles[filenum])
			{
				f = &level_openScriptIOFileHandles[filenum];
				break;
			}
		}

		if (f)
		{
			char *fullpathname = va("%s/%s", "scriptdata", filename);

			//
			// Prevent Scripts from Opening Files Outside of the ScriptData Directory
			//
			char* possibleRelativePath = strstr((char*)filename, "..");
			if (possibleRelativePath == filename && strlen(possibleRelativePath) >= 3)
			{
				if (possibleRelativePath[2] == '/' || possibleRelativePath[2] == '\\')
				{
					Com_Printf(24, "OpenFile failed.  Relative paths are not allowed!\n");
					Scr_AddInt(-1, SCRIPTINSTANCE_SERVER);
				}
			}

			if (!strcmp(mode, "read"))
			{
				int tempFile = NULL;
				int filesize = FS_FOpenFileByMode(fullpathname, &tempFile, FS_READ);

				if (filesize >= 0)
				{
					level_openScriptIOFileBuffers[filenum] = (BYTE*)malloc(filesize + 1);
					FS_Read(level_openScriptIOFileBuffers[filenum], filesize, tempFile);
					FS_FCloseFile(tempFile);
					level_openScriptIOFileBuffers[filenum][filesize] = 0;
					
					Com_BeginParseSession(filename);
					//Com_SetCSV(1);
					
					//dword_3E53D10[5 * filenum] = 0;
					
					Scr_AddInt(filenum, SCRIPTINSTANCE_SERVER);
				}
				else
				{
					Scr_AddInt(-1, SCRIPTINSTANCE_SERVER);
				}
			}
			else if (!strcmp(mode, "write"))
			{
				*f = FS_FOpenTextFileWrite(fullpathname);
				if (*f)
					Scr_AddInt(filenum, SCRIPTINSTANCE_SERVER);
				else
					Scr_AddInt(-1, SCRIPTINSTANCE_SERVER);
			}
			else if (!strcmp(mode, "append"))
			{
				if (FS_FOpenFileByMode(fullpathname, f, FS_APPEND) >= 0)
					Scr_AddInt(filenum, SCRIPTINSTANCE_SERVER);
				else
					Scr_AddInt(-1, SCRIPTINSTANCE_SERVER);
			}
			else
			{
				Com_Printf(24, "Valid openfile modes are 'write', 'read', and 'append'\n");
				Scr_AddInt(-1, SCRIPTINSTANCE_SERVER);
			}
		}
		else
		{
			Com_Printf(24, "OpenFile failed.  %i files already open\n", 1);
			Scr_AddInt(-1, SCRIPTINSTANCE_SERVER);
		}
	}
}

void CScr_CloseFile()
{
	if (!developer->current.enabled || !developer_script->current.enabled)
	{
		Scr_AddInt(-1, SCRIPTINSTANCE_CLIENT);
	}

	if (Scr_GetNumParam(SCRIPTINSTANCE_CLIENT))
	{
		int filenum = Scr_GetInt(0, SCRIPTINSTANCE_CLIENT);
		if (filenum < 0 || filenum > 1)
		{
			Com_Printf(24, "CloseFile failed, invalid file number %i\n", filenum);
			Scr_AddInt(-1, SCRIPTINSTANCE_CLIENT);
			return;
		}

		//ASSERT(!client_openScriptIOFileHandles[filenum] && !client_openScriptIOFileBuffers[filenum]);

		if (client_openScriptIOFileHandles[filenum])
		{
			FS_FCloseFile((int)client_openScriptIOFileHandles[filenum]);
			client_openScriptIOFileHandles[filenum] = 0;
		}
		else
		{
			if (!client_openScriptIOFileBuffers[filenum])
			{
				Com_Printf(24, "CloseFile failed, file number %i was not open\n", filenum);
				Scr_AddInt(-1, SCRIPTINSTANCE_CLIENT);
				return;
			}

			Com_EndParseSession();

			free(client_openScriptIOFileBuffers[filenum]);
			client_openScriptIOFileBuffers[filenum] = 0;
		}
		Scr_AddInt(1, SCRIPTINSTANCE_CLIENT);
	}
}

void GScr_CloseFile()
{
	if (!developer->current.enabled || !developer_script->current.enabled)
	{
		Scr_AddInt(-1, SCRIPTINSTANCE_SERVER);
	}

	if (Scr_GetNumParam(SCRIPTINSTANCE_SERVER))
	{
		int filenum = Scr_GetInt(0, SCRIPTINSTANCE_SERVER);
		if (filenum < 0 || filenum > 1)
		{
			Com_Printf(24, "CloseFile failed, invalid file number %i\n", filenum);
			Scr_AddInt(-1, SCRIPTINSTANCE_SERVER);
			return;
		}
		
		//ASSERT(!level_openScriptIOFileHandles[filenum] && !level_openScriptIOFileBuffers[filenum]);

		if (level_openScriptIOFileHandles[filenum])
		{
			FS_FCloseFile((int)level_openScriptIOFileHandles[filenum]);
			level_openScriptIOFileHandles[filenum] = 0;
		}
		else
		{
			if (!level_openScriptIOFileBuffers[filenum])
			{
				Com_Printf(24, "CloseFile failed, file number %i was not open\n", filenum);
				Scr_AddInt(-1, SCRIPTINSTANCE_SERVER);
				return;
			}

			Com_EndParseSession();

			free(level_openScriptIOFileBuffers[filenum]);
			level_openScriptIOFileBuffers[filenum] = 0;
		}
		Scr_AddInt(1, SCRIPTINSTANCE_SERVER);
	}
}

void __cdecl CScr_FPrint_internal(bool commaBetweenFields)
{
	if (!developer->current.enabled || !developer_script->current.enabled)
	{
		Scr_AddInt(-1, SCRIPTINSTANCE_CLIENT);
	}

	if (Scr_GetNumParam(SCRIPTINSTANCE_CLIENT) > 1)
	{
		int filenum = Scr_GetInt(0, SCRIPTINSTANCE_CLIENT);
		if (filenum >= 0 && filenum <= 1)
		{
			if (client_openScriptIOFileHandles[filenum])
			{
				for (unsigned int arg = 1; arg < Scr_GetNumParam(SCRIPTINSTANCE_CLIENT); ++arg)
				{
					const char* s = Scr_GetString(arg, SCRIPTINSTANCE_CLIENT);
					FS_Write(s, strlen(s), client_openScriptIOFileHandles[filenum]);

					if (commaBetweenFields)
						FS_Write(",", 1, client_openScriptIOFileHandles[filenum]);
				}

				FS_Write("\n", 1, client_openScriptIOFileHandles[filenum]);

				int val = Scr_GetNumParam(SCRIPTINSTANCE_CLIENT);
				Scr_AddInt(val - 1, SCRIPTINSTANCE_CLIENT);
			}
			else
			{
				Com_Printf(24, "FPrintln failed, file number %i was not open for writing\n", filenum);
				Scr_AddInt(-1, SCRIPTINSTANCE_CLIENT);
			}
		}
		else
		{
			Com_Printf(24, "FPrintln failed, invalid file number %i\n", filenum);
			Scr_AddInt(-1, SCRIPTINSTANCE_CLIENT);
		}
	}
	else
	{
		Com_Printf(24, "fprintln requires at least 2 parameters (file, output)\n");
		Scr_AddInt(-1, SCRIPTINSTANCE_CLIENT);
	}
}

void __cdecl Scr_FPrint_internal(bool commaBetweenFields)
{
	if (!developer->current.enabled || !developer_script->current.enabled)
	{
		Scr_AddInt(-1, SCRIPTINSTANCE_SERVER);
	}

	if (Scr_GetNumParam(SCRIPTINSTANCE_SERVER) > 1)
	{
		int filenum = Scr_GetInt(0, SCRIPTINSTANCE_SERVER);
		if (filenum >= 0 && filenum <= 1)
		{
			if (level_openScriptIOFileHandles[filenum])
			{
				for (unsigned int arg = 1; arg < Scr_GetNumParam(SCRIPTINSTANCE_SERVER); ++arg)
				{
					const char* s = Scr_GetString(arg, SCRIPTINSTANCE_SERVER);
					FS_Write(s, strlen(s), level_openScriptIOFileHandles[filenum]);
					
					if (commaBetweenFields)
						FS_Write(",", 1, level_openScriptIOFileHandles[filenum]);
				}
				
				FS_Write("\n", 1, level_openScriptIOFileHandles[filenum]);
				
				int val = Scr_GetNumParam(SCRIPTINSTANCE_SERVER);
				Scr_AddInt(val - 1, SCRIPTINSTANCE_SERVER);
			}
			else
			{
				Com_Printf(24, "FPrintln failed, file number %i was not open for writing\n", filenum);
				Scr_AddInt(-1, SCRIPTINSTANCE_SERVER);
			}
		}
		else
		{
			Com_Printf(24, "FPrintln failed, invalid file number %i\n", filenum);
			Scr_AddInt(-1, SCRIPTINSTANCE_SERVER);
		}
	}
	else
	{
		Com_Printf(24, "fprintln requires at least 2 parameters (file, output)\n");
		Scr_AddInt(-1, SCRIPTINSTANCE_SERVER);
	}
}

void CScr_FPrintln()
{
	CScr_FPrint_internal(0);
}

void GScr_FPrintln()
{
	Scr_FPrint_internal(0);
}

void CScr_FPrintFields()
{
	CScr_FPrint_internal(1);
}

void GScr_FPrintFields()
{
	Scr_FPrint_internal(1);
}

