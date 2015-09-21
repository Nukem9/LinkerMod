#include "stdafx.h"

FILE** fDeps = (FILE**)0x010133C8;
void* sub_41DDD0 = (void*)0x0041DDD0;

void* rtn_fcloseDeps = (void*)0x0041E68F;
void __declspec(naked) mfh_fcloseDeps()
{
	_asm
	{
		push edx
		mov edx, [fDeps]
		push edx
		call o_fclose
		add esp, 4
		pop edx
		call sub_41DDD0
		push ebx
		jmp rtn_fcloseDeps
	}
}