#include "cmd_common.h"
#include "../../cache/cache.h"

int Cmd_Cache_Clear_f(int argc, char** argv)
{
	Cache_Clear();	
	return 0;
}
