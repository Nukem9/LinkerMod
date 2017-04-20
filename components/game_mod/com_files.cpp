#include "com_files.h"
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
