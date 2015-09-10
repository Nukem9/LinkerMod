#include "stdafx.h"

void (__cdecl * Com_Init)(char *commandLine);

void hk_Com_Init(char *commandLine)
{
	//
	// Fix up the command line because devs removed it
	//
	commandLine = (char *)0x276D0D8;

	Com_Init(commandLine);
}

void Field_Clear(field_t *edit)
{
	((void(__cdecl *)(field_t *))0x0050DB60)(edit);
}

int nullsub(int arg)
{
	return 0;
}