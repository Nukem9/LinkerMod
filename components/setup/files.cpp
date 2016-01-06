#include "files.h"
#include <windows.h>
#include <string>

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