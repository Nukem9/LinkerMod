#include "stdafx.h"

// /clientscript/cscr_main.cpp:34
int Scr_IsInOpcodeMemory(scriptInstance_t inst, const char *pos)
{
	ASSERT(gScrVarPub[inst].programBuffer);
	ASSERT(pos);

	return ((unsigned)(pos - gScrVarPub[inst].programBuffer)) < gScrCompilePub[inst].programLen;
}