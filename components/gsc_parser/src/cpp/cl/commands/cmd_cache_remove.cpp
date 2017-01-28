#include "cmd_common.h"
#include "../../cache/cache.h"

int Cmd_Cache_Remove_f(int argc, char** argv)
{
	if(argc <= 1)
	{
		fprintf(stderr, "Invalid number of arguments\n");
		return 1;
	}
	
	for(int i = 1; i < argc; i++)
	{
		Cache_Remove(argv[i]);	
	}
	
	return 0;
}
