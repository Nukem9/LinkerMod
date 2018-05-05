#include "stdafx.h"

// /clientscript/cscr_variable.cpp:???
void Scr_GetChecksumNoAsserts(int *checksum, scriptInstance_t inst)
{
	checksum[0] = gScrVarPub[inst].checksum;
	checksum[1] = gScrCompilePub[inst].programLen;
	checksum[2] = gScrVarPub[inst].endScriptBuffer - gScrVarPub[inst].programBuffer;
}