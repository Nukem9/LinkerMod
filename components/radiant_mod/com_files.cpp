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
			sprintf_s(npath, PATH_PIMP_FALLBACK"/%s", qpath);
			_VERBOSE( printf("%s\n", npath) );
			result = o_FS_ReadFile(npath, buffer);
		}
	}

	return result;
}

#include "r_material_load_obj.h"

void FS_Init_TechsetOverride(void)
{
	FILE* h = fopen("techset_override.csv", "r");

	if (h)
	{
		for (int eof = false; !eof && !feof(h);)
		{
			char buf[1024] = "";
			if (!fgets(buf, 1024, h))
			{
				fclose(h);
				return;
			}

			techsetOverride tsOverride;
			char* p = strtok(buf, " \t\n,");
			if (!p || (p[0] == '/' && p[1] == '/'))
			{
				continue;
			}
			tsOverride.key = p;

			p = strtok(NULL, " \t\n,");
			if (!p)
			{
				continue;
			}
			tsOverride.replacement = p;

			techsetOverrideList.push_back(tsOverride);
		}

		fclose(h);
	}
}

int __cdecl FS_HashFileName(const char *fname, int hashSize)
{
	int hash = 0;
	for (int i = 0; fname[i]; ++i)
	{
		int letter = tolower(fname[i]);
		if (letter == '.')
			break;
		if (letter == '\\')
			letter = '/';
		hash += letter * (i + 119);
	}

	return ((hash >> 20) ^ hash ^ (hash >> 10)) & (hashSize - 1);
}
