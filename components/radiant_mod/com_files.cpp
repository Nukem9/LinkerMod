#include "stdafx.h"

FS_ReadFile_t o_FS_ReadFile = (FS_ReadFile_t)0x004BC840;

int __cdecl FS_ReadFile(const char *qpath, void **buffer)
{
	int result = o_FS_ReadFile(qpath, buffer);

	if (result < 0)
	{
		if (strncmp(qpath, "techniques", strlen("techniques")) == 0 || strncmp(qpath, "techsets", strlen("techsets")) == 0)
		{
			char npath[MAX_PATH];
			sprintf_s(npath, "waw_pimp/%s", qpath);
			printf("%s\n", npath);
			result = o_FS_ReadFile(npath, buffer);
		}
	}

	return result;
}