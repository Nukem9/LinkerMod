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

	if (cmd == &g_cmd_csvgen)
	{
		Con_Print("Info:\n"
			"  csvgen has two 'modes' which control how it runs.\n\n"
			"  auto mode: csvgen [options] [*]\n"
			"    Options determine which types csvgen tries to regenerate csvs for\n"
			"    by default all types are enabled\n\n"
			"  explicit mode: csvgen [option] file1 [file2 ...]\n"
			"    Command specific options define the type of file(s) being passed to csvgen\n"
			"    If no type option is given, or more than one is given csvgen will attempt\n"
			"    to automatically determine the type for each file\n"
			"\n");
	}

	return 0;
}