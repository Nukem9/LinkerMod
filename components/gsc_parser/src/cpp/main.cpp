#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
	#include <Windows.h>
#endif

#include "sys/sys_platform.h"
#include "sys/sys_worker.h"
#include "sys/sys_cpu.h"
#include "cl/cl_arg.h"
#include "cl/cl_cvar.h"
#include "cl/cl_cmd.h"

int main(int argc, char** argv)
{
	if(argc <= 1)
	{
		Arg_PrintUsage();
		return 1;
	}
	
	ArgParsedInfo cmd_info;
	if(int err = Arg_ParseArguments(argc - 1, argv + 1, &cmd_info))
	{
		fprintf(stderr, "Fatal Error: %d\n", err);
		return err;
	}
	
	return cmd_info.Cmd()->Exec(cmd_info.Argc(), cmd_info.Argv());
}