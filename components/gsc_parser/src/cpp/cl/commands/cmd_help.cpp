#include "cmd_common.h"
#include "../cl_arg.h"

int Cmd_Help_f(int argc, char** argv)
{
	Arg_PrintUsage();
	return 0;
}