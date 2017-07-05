#include "cl_watch_mode.h"

bool g_watch_mode = false;

void CL_WatchMode_Enable(void)
{
	g_watch_mode = true;
}

void CL_WatchMode_Disable(void)
{
	g_watch_mode = false;
}

bool CL_WatchMode_IsEnabled(void)
{
	return g_watch_mode;
}
