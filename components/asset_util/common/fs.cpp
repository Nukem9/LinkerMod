#pragma once
#include "fs.h"
#include "../sys/AppInfo.h"
#include "../common/io.h"
#include <sys/stat.h>
#include <Windows.h>

const char* FS_Cwd(void)
{
	static char cwd[MAX_PATH] = "\0";
	if (!cwd[0])
	{
		GetCurrentDirectoryA(MAX_PATH, cwd);
	}

	return cwd;
}

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

int FS_FileCount(const char* path, const char* pattern)
{
	HANDLE dir;
	WIN32_FIND_DATAA file_data;

	char spath[MAX_PATH];
	sprintf_s(spath, "%s/%s", path, pattern);
	Con_Print("%s\n", spath);

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
	} while (FindNextFileA(dir, &file_data));

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

int FS_DirectoryIterator(const char* path, int(__cdecl* FS_DirectoryHandlerCallback)(const char* path))
{
	HANDLE dir;
	WIN32_FIND_DATAA file_data;

	char qpath[MAX_PATH];
	sprintf(qpath, "%s/*", path);

	if ((dir = FindFirstFileA(qpath, &file_data)) == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	int count = 0;
	do
	{
		if (file_data.cFileName[0] == '.')
		{
			continue;
		}

		if (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			char tpath[MAX_PATH];
			sprintf(tpath, "%s/%s", path, file_data.cFileName);

			FS_DirectoryHandlerCallback(tpath);
			count++;
		}
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

int FS_CopyDirectory(char* srcPath, char* destPath, bool overwriteFiles)
{
	WIN32_FIND_DATAA file_data;
	HANDLE h;
	char tmp[1025] = "\0";
	strcpy(tmp, srcPath);

	char tmp_srcPath[1025] = { 0 };
	char tmp_destPath[1025] = { 0 };

	strcat(tmp, "*");

	if (!(h = FindFirstFileA(tmp, &file_data)))
	{
		return 1;
	}

	do
	{
		if (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(file_data.cFileName, ".") && strcmp(file_data.cFileName, ".."))
			{
				sprintf(tmp_destPath, "%s%s\\", destPath, file_data.cFileName);
				sprintf(tmp_srcPath, "%s%s\\", srcPath, file_data.cFileName);

				CreateDirectoryA(tmp_destPath, NULL);
				FS_CopyDirectory(tmp_srcPath, tmp_destPath, overwriteFiles);
			}
		}
		else
		{
			char srcFile[1025] = "\0";
			char destFile[1025] = "\0";

			sprintf(destFile, "%s%s", destPath, file_data.cFileName);
			sprintf(srcFile, "%s%s", srcPath, file_data.cFileName);

			CopyFileA(srcFile, destFile, !overwriteFiles);
		}
	} while (FindNextFileA(h, &file_data));

	FindClose(h);
	return 0;
}

