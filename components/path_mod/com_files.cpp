#include "stdafx.h"

FS_ReadFile_t FS_ReadFile_o = (FS_ReadFile_t)0x0054EC65;
fopen_t fopen_o = (fopen_t)0x006C5F61;

int __cdecl FS_ReadFile(const char *qpath, void **buffer)
{
	int result = 0;

	if (strncmp(qpath, "techsets", strlen("techsets")) == 0 || strncmp(qpath, "techniques", strlen("techniques")) == 0)
	{
		char npath[MAX_PATH];
		sprintf_s(npath, "waw_pimp/%s", qpath);
		result = FS_ReadFile_o(npath, buffer);
	}
	else
	{
		result = FS_ReadFile_o(qpath, buffer);
	}

	return result;
}

FILE *__cdecl fopen_custom(const char* filename, const char* mode)
{
	char* substr = (char*)filename;

	if (substr = (char*)strstr(filename, "\\ui_mp\\"))
	{
		substr[0] = 0;
		substr += strlen("\\ui_mp\\");
		
		char buf[MAX_PATH];
		sprintf_s(buf, "%s\\waw_ui_mp\\%s", filename, substr);
		return fopen_o(buf, mode);
	}
	else if (substr = (char*)strstr(filename, "\\ui\\"))
	{
		substr[0] = 0;
		substr += strlen("\\ui\\");

		char buf[MAX_PATH];
		sprintf_s(buf, "%s\\waw_ui\\%s", filename, substr);
		return fopen_o(buf, mode);
	}

	return fopen_o(filename, mode);
}
