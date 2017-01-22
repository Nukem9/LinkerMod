#include "stdafx.h"

static int& g_progressCounter_current = *(int*)0x1730241C;
static int& g_progressCounter_max = *(int*)0x17302420;

void __cdecl SetProgress(int current, int total)
{
	g_progressCounter_current = current;
	g_progressCounter_max = total;

	UpdateProgressPrint();
}

void __cdecl UpdateProgress(int amount)
{
	if (g_progressCounter_max)
	{
		g_progressCounter_current += amount;
		UpdateProgressPrint();
	}
}
