#include "stdafx.h"

int __cdecl hk_FS_ReadFile(const char *qpath, void **buffer)
{
	int result = o_FS_ReadFile(qpath, buffer);

	if (result < 0)
	{
		if( strncmp(qpath, "techsets", strlen("techsets")) == 0 || strncmp(qpath, "techniques", strlen("techniques")) == 0)
		{
			char npath[MAX_PATH];
			sprintf_s(npath, "waw_pimp/%s", qpath);

			result = o_FS_ReadFile(npath, buffer);
		}
	}

	return result;
}