#pragma once
#include "fs.h"
#include <sys/stat.h>
#include <Windows.h>

bool FS_FileExists(const char* qpath)
{
	FILE* h = NULL;
	if (fopen_s(&h, qpath, "r") == NULL)
	{
		fclose(h);
		return true;
	}

	return false;
}

size_t FS_FileSize(const char* qpath)
{
	struct stat st_buf;
	int r = stat(qpath, &st_buf);
	return r == 0 ? st_buf.st_size : -1;
}

const char* FS_GetExtensionSubString(const char* filename)
{
	_ASSERT(filename != NULL);

	const char* substr = 0;
	while (*filename)
	{
		if (*filename == '.')
			substr = filename;
		else if (*filename == '/' || *filename == '\\')
			substr = 0;

		++filename;
	}

	if (!substr)
		substr = filename;

	return substr;
}

void FS_StripExtension(const char* in, char* out)
{
	const char* extension = FS_GetExtensionSubString(in);
	while (in != extension)
		*out++ = *in++;
	*out = 0;
}

const char* FS_GetFilenameSubString(const char* pathname)
{
	const char* last = pathname;
	while (*pathname)
	{
		if (*pathname == '/' || *pathname == '\\')
			last = pathname + 1;
		++pathname;
	}
	return last;
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
