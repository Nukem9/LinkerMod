#include "stdafx.h"
#include <iostream>

char dirList[DIRLIST_LEN];

FS_ModDesc::FS_ModDesc(void)
{
	this->Clear();
}

void FS_ModDesc::Clear(void)
{
	this->type = DESC_ERROR;
	ZeroMemory(this->data, bufLen);
}

size_t FS_ModDesc::Length(void) const
{
	return strlen((const char*)this) + 1;
}

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

	char descPath[MAX_PATH];
	sprintf_s(descPath, "%s/%s", basepath, "mods");

	int dummy = 0;
	char** pFiles = Sys_ListFiles(descPath, 0, 0, &dummy, 1);
	nPotential = Sys_CountFileList(pFiles);
	for (int i = 0; i < nPotential; ++i)
	{
		char* name = pFiles[i];
		int nLen = strlen(name) + 1;

		//
		// Try to load mod.json first
		//
		sprintf_s(descPath, "%s%s", name, "/mod.json");

		FS_ModDesc desc;

		int descHandle = 0;
		if (FS_SV_FOpenFileRead(descPath, "mods", &descHandle, NULL) > 0 && descHandle)
		{
			_iobuf* file = FS_FileForHandle(descHandle);
			desc.Clear();
			int nDescLen = FS_ReadModDescription(desc.data, desc.bufLen, file);

			// Ensure that the string is null terminated
			if (nDescLen >= 0)
				desc.data[nDescLen] = '\0';

			desc.type = DESC_JSON;
			FS_FCloseFile(descHandle);
		}
		else
		{
			//
			// Otherwise fallback to legacy description.txt
			//
			sprintf_s(descPath, "%s%s", name, "/description.txt");

			int descHandle = 0;
			if (FS_SV_FOpenFileRead(descPath, "mods", &descHandle, NULL) > 0 && descHandle)
			{
				_iobuf* file = FS_FileForHandle(descHandle);
				desc.Clear();
				int nDescLen = FS_ReadModDescription(desc.data, desc.bufLen, file);
				
				// Ensure that the string is null terminated
				if (nDescLen >= 0)
					desc.data[nDescLen] = '\0';
				
				desc.type = DESC_DESC;
				FS_FCloseFile(descHandle);
			}
			else
			{
				Com_Printf(10, "FS_GetModList: failed to open '%s' or '%s' for mod '%s'\n", "mod.json", "description.txt", name);
				desc.Clear();
			}
		}

		int nDescLen = desc.Length();
		if (nLen + nTotal + nDescLen + 2 >= bufsize)
			break;

#pragma warning( push )
#pragma warning( disable : 4996 )
		strcpy(listbuf, name);
		strcpy(&listbuf[nLen], (char*)&desc);
#pragma warning( pop )  

		listbuf = &listbuf[nLen + nDescLen];
		nTotal += nDescLen + nLen;
		++nMods;
	}

	FS_FreeFileList((const char**)pFiles, 0);
	return nMods;
}

typedef int(__cdecl* FS_FOpenFileWriteToDir_t)(const char *filename, const char *dir, const char *osbasepath);
FS_FOpenFileWriteToDir_t FS_FOpenFileWriteToDir_o = (FS_FOpenFileWriteToDir_t)0x00625D90;

int __cdecl FS_FOpenFileWriteToDir(const char *filename, const char *dir, const char *osbasepath)
{
	if (com_cfg_readOnly && com_cfg_readOnly->current.enabled)
		return 0;

	return FS_FOpenFileWriteToDir_o(filename, dir, osbasepath);
}
