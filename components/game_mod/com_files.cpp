#include "stdafx.h"
#include <iostream>

char dirList[DIRLIST_LEN];

unsigned int __cdecl FS_ReadModDescription(void *ptr, unsigned int len, _iobuf *stream)
{
	return FS_FileRead(ptr, MODDESC_LEN, stream);
}

void __declspec(naked) mfh_FS_GetModList(void)
{
	static void* dwRetn = (void*)0x00455A34;

	_asm
	{
		push esi
		lea edx, [esp + 0x44]
		push MODDESC_LEN
		push edx
		jmp dwRetn;
	}
}

unsigned int __cdecl Sys_CountFileList(char **list)
{
	int i = 0;
	if (list)
	{
		while (*list)
		{
			++list;
			++i;
		}
	}
	return i;
}

int __cdecl FS_GetModList(char *listbuf, int bufsize)
{
	*listbuf = 0;

	int nTotal = 0;
	int nPotential = 0;
	int nMods = 0;

	const char* basepath = fs_homepath->current.string;

	char descPath[256];
	sprintf_s(descPath, 256, "%s/%s", basepath, "mods");

	int dummy = 0;
	char** pFiles = Sys_ListFiles(descPath, 0, 0, &dummy, 1);
	nPotential = Sys_CountFileList(pFiles);
	for (int i = 0; i < nPotential; ++i)
	{
		char* name = pFiles[i];
		int nLen = strlen(name) + 1;

		strcpy(descPath, name);

		strncat_s(descPath, "/description.txt", 256 );

		int descHandle = 0;
		if (FS_SV_FOpenFileRead(descPath, "mods", &descHandle) > 0 && descHandle)
		{
			_iobuf* file = FS_FileForHandle(descHandle);
			memset(descPath, 0, 256);
			int nDescLen = FS_FileRead(descPath, 48u, file);
			if (nDescLen >= 0)
				descPath[nDescLen] = 0;
			FS_FCloseFile(descHandle);
		}
		else
		{
			Com_Printf(10, "FS_GetModList: failed to open %s\n", descPath);
			descPath[0] = '\0';
		}

		int nDescLen = strlen(descPath) + 1;
		if (nLen + nTotal + nDescLen + 2 >= bufsize)
			break;

		strcpy(listbuf, name);
		strcpy(&listbuf[nLen], descPath);

		listbuf = &listbuf[nLen + nDescLen];
		nTotal += nDescLen + nLen;
		++nMods;
	}

	FS_FreeFileList((const char**)pFiles, 0);
	return nMods;
}
