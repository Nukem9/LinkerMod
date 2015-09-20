#include "stdafx.h"
#include <direct.h>

FS_FileOpen_t FS_FileOpen = (FS_FileOpen_t)0x004034E8;

size_t FS_FileGetFileSize(FILE* h)
{
	int loc = FS_FTell(h);
	FS_FSeek(h, 0, SEEK_END);

	size_t size = FS_FTell(h);
	FS_FSeek(h, loc, SEEK_SET);

	return size;
}

size_t FS_FOpenFileRead(const char* qpath, void** buffer)
{
	*buffer = nullptr;

	if (!qpath || !*qpath)
		return -1;

	FILE* h = FS_FileOpen(qpath, "rb");
	if (h)
	{
		size_t len = FS_FileGetFileSize(h);
		BYTE* buf = (BYTE*)FS_AllocMem(len + 1);
		buf[len] = 0;
		if (len)
			FS_FileRead(buf, 1, len, h);
		FS_FileClose(h);

		len = Com_LoadBsp_EnforceVersion(&buf);
		*buffer = buf;

		return len;
	}

	return -1;
}

void __declspec(naked) hk_FS_FOpenFileRead()
{
	_asm
	{
		push [esp + 4]	//buffer
		push edx		//qpath
		call FS_FOpenFileRead
		add esp, 8
		retn
	}
}

const char* strlastof(const char* str, const char* delims)
{
	int delimCount = strlen(delims);

	for (int i = strlen(str); i >= 0; i--)
	{
		for (int d = 0; d < delimCount; d++)
		{
			if (str[i] == delims[d])
				return str + i;
		}
	}

	return nullptr;
}

FILE* __cdecl FS_ImageRedirect(const char* filename, const char* mode)
{
	FILE* h = FS_FileOpen(filename, mode);

	if (!h)
	{
		if (*(DWORD*)(filename + strlen(filename) - 4) != 'iwi.')
			return h;

		char qpath[MAX_PATH];
		_getcwd(qpath, MAX_PATH);

		(char)strlastof(qpath, "\\/")[1] = 0;
		strcat_s(qpath, "raw\\images\\");
		strcat_s(qpath, strlastof(filename, "\\/") + 1);
		h = FS_FileOpen(qpath, mode);
	}

	return h;
}