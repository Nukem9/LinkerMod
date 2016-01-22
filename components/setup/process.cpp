#include "process.h"
#include "steam.h"

int Process_ExecuteConverter(void)
{
	char wd[MAX_PATH];
	sprintf_s(wd, "%s/bin/", AppInfo_AppDir());

	char szCmdLine[MAX_PATH] = "\0";
	sprintf_s(szCmdLine, "%s/converter.exe -nopause", wd);

	PROCESS_INFORMATION processInfo;
	STARTUPINFOA startupInfo;
	memset(&startupInfo, 0, sizeof(STARTUPINFOA));
	startupInfo.cb = sizeof(STARTUPINFOA);

	if (CreateProcessA(NULL, szCmdLine, NULL, NULL, NULL, NULL, NULL, wd, &startupInfo, &processInfo))
	{
		WaitForSingleObject(processInfo.hProcess, INFINITE);
		CloseHandle(processInfo.hThread);
		CloseHandle(processInfo.hProcess);

		return 0;
	}

	return 1;
}