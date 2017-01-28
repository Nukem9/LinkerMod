#include "cmd_common.h"
#include "../cl_watch_mode.h"
#include "../../cache/cache.h"

int Cmd_Exit_f(int argc, char** argv)
{
	Cache_Clear();
	CL_WatchMode_Disable();
	return 0;
}
