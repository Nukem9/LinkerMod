#include "stdafx.h"

char cfg_default_path[] = "/config.cfg";
void* Cmd_ExecFromDisk = (void*)0x008295D0;

void* rtn_Cmd_Exec_f = (void*)0x0082A26E;
void __declspec(naked) mfh_Cmd_Exec_f()
{
	_asm
	{
		push ecx
		push esi
		push 0
		push 0
		call Cmd_ExecFromDisk
		cmp al, 0
		je CONFIGFIX
		add esp, 16
		jmp rtn_Cmd_Exec_f

	CONFIGFIX:
		add esp, 8
		pop esi
		pop ecx
		lea esi, cfg_default_path
		call Cmd_ExecFromDisk
		jmp rtn_Cmd_Exec_f
	}
}
