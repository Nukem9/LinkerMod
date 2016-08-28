#include "stdafx.h"

#define ARENA_FILE_MAX_SIZE 1024

int* ui_numArenas = (int*)0x025F6940;

bool __cdecl UI_LoadModArenas()
{
	int file = NULL;
	size_t fileSize = FS_FOpenFileRead("mod.arena", &file);
	
	if(file)
	{
		if(fileSize)
		{
			if(fileSize <= ARENA_FILE_MAX_SIZE)
			{
				char buf[ARENA_FILE_MAX_SIZE];
				
				FS_Read(buf, fileSize, file);
				FS_FCloseFile(file);

				*ui_numArenas = UI_ParseInfos(buf, 128 - *ui_numArenas, (char **)(4 * *ui_numArenas + 0x98A4B50));
				return true;
			}
			else
			{
				Com_PrintWarning(13, "Customized arena file size is too big to load > %d: %s\n", ARENA_FILE_MAX_SIZE, "mod.arena");
				FS_FCloseFile(file); //Fix for leaked handles
				return false;
			}
		}
		else
		{
			Com_PrintWarning(13, "Customized arena file is empty: %s\n", "mod.arena");
			FS_FCloseFile(file); //Fix for leaked handles
			return false;
		}
	}
	else
	{
		Com_PrintWarning(13, "Customized arena file not found: %s\n", "mod.arena"); 
		return false;
	}
}