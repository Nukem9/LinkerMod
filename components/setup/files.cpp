#include "files.h"
#include <windows.h>
#include <string>
#include "steam.h"

int FS_FileCount(const char* path, const char* pattern)
{
	HANDLE dir;
	WIN32_FIND_DATAA file_data;

	char spath[MAX_PATH];
	sprintf_s(spath, "%s/%s", path, pattern);
	printf("%s\n", spath);

	if ((dir = FindFirstFileA(spath, &file_data)) == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	int count = 0;
	do
	{
		if ((file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			continue;
		}

		count++;
	}
	while (FindNextFileA(dir, &file_data));

	FindClose(dir);
	return count;
}

int FS_FileIterator(const char* path, const char* pattern, int(__cdecl* FS_FileHandlerCallback)(const char* filePath, const char* fileName))
{
	HANDLE dir;
	WIN32_FIND_DATAA file_data;

	char spath[MAX_PATH];
	sprintf_s(spath, "%s/%s", path, pattern);

	if ((dir = FindFirstFileA(spath, &file_data)) == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	int count = 0;
	do
	{
		if ((file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			continue;
		}

		char fpath[MAX_PATH];
		sprintf_s(fpath, "%s/%s", path, file_data.cFileName);
		FS_FileHandlerCallback(fpath, file_data.cFileName);

		count++;
	} while (FindNextFileA(dir, &file_data));

	FindClose(dir);
	return count;
}

int FS_CreatePath(const char* targetPath)
{
	int len = strlen(targetPath);
	for (int i = 0; i < len; i++)
	{
		if (targetPath[i] == '/' || targetPath[i] == '\\')
		{
			char buf[1024] = "";
			strncpy(buf + strlen(buf), targetPath, i);

			char qpath[1024] = "";
			sprintf_s(qpath, "%s/%s/", AppInfo_RawDir(), buf);

#if _DEBUG
			if (!CreateDirectoryA(buf, 0) && GetLastError() != ERROR_ALREADY_EXISTS)
#else
			if (!CreateDirectoryA(qpath, 0) && GetLastError() != ERROR_ALREADY_EXISTS)
#endif
			{
				return GetLastError();
			}
		}
	}

	return 0;
}