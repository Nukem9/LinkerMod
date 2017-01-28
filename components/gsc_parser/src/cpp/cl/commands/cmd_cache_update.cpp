#include "cmd_common.h"
#include "../../cache/cache.h"
#include <stdio.h>

int Cmd_Cache_Update_f(int argc, char** argv)
{
	if(argc <= 1)
	{
		fprintf(stderr, "Invalid number of arguments\n");
		return 1;
	}
	
	for(int i = 1; i < argc; i++)
	{
		Cache_Update(argv[i]);	
	}
	
	return 0;
}
