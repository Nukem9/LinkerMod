#include "stdafx.h"
#include "common.h"

void Com_SuppressNoModelSpam(int channel, const char* fmt, const char* name)
{
	if (*name)
	{
		Com_PrintError(channel, fmt, name);
	}
}
