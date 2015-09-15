#include "stdafx.h"

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