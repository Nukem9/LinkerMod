#include "cmd_common.h"
#include "../cmd.h"
#include "../arg.h"

int Cmd_Help_f(int argc, char** argv)
{
	if (argc <= 1)
	{
		Arg_PrintUsage();
		return 0;
	}

	Command* cmd = Command::ResolveCommand(argv[1]);
	if (cmd == NULL)
	{
		Con_Error("ERROR: Unrecognized command '%s'\n", argv[1]);
		return 1;
	}

	Arg_PrintUsage(cmd);
	return 0;
}