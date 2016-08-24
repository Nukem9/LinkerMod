#include "setup.h"
#include "files.h"
#include "process.h"
#include "io.h"
#include <Windows.h>
#include "AppInfo.h"

int Setup_Init(void)
{
	return 0;
}

int Setup_Execute(void)
{
	char srcPath[MAX_PATH] = "\0";
	sprintf_s(srcPath, "%s\\data\\", FS_Cwd());

	char destPath[MAX_PATH] = "\0";
	sprintf_s(destPath, "%s\\", AppInfo_AppDir());

	FS_CopyDirectory(srcPath, destPath, true);

	return 0;
}
