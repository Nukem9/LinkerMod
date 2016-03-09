#include "setup.h"
#include "files.h"
#include "process.h"
#include "io.h"
#include "steam.h"
#include <Windows.h>

int Setup_Init(void)
{
	return 0;
}

int Setup_Execute(void)
{
	char mapSrcPath[MAX_PATH] = "\0";
	sprintf_s(mapSrcPath, "%s\\map_source\\.game", AppInfo_AppDir());

	if (!FS_TestPath(mapSrcPath))
	{
		sprintf_s(mapSrcPath, "%s\\map_source\\", AppInfo_AppDir());

		char mapDestPath[MAX_PATH] = "\0";
		sprintf_s(mapDestPath, "%s\\_map_cache\\", AppInfo_AppDir());

		MoveFileA(mapSrcPath, mapDestPath);
	
		strcat(mapSrcPath, ".game\\");
		FS_CreatePath("..\\map_source\\");
		MoveFileA(mapDestPath, mapSrcPath);
	}

	char srcPath[MAX_PATH] = "\0";
	sprintf_s(srcPath, "%s\\data\\", FS_Cwd());

	char destPath[MAX_PATH] = "\0";
	sprintf_s(destPath, "%s\\", AppInfo_AppDir());

	FS_CopyDirectory(srcPath, destPath, true);
	return 0;
}
