#include "cmd_common.h"
#include "../../cache/cache.h"

int Cmd_Cache_List_f(int argc, char** argv)
{
	Cache_List();
	return 0;
}
