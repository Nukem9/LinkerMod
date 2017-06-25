#pragma once
#include "cl_arg.h"
#include "../util/llist.h"

typedef int (*cmd_func_t)(int argc, char** argv);

enum COMMAND_FLAGS
{
	COMMAND_LAUNCH	= 1 << 0,	// Launch command
	COMMAND_WATCH	= 1 << 1,	// Watch mode command
};

class Command : public Argument, public LList<Command>
{
private:
	static Command* g_cmds;
	cmd_func_t func;
	int cmd_flags;
	
public:

	Command(const char* name, const char* description, cmd_func_t func, int cmd_flags);
	
	int Exec(int argc, char** argv) const;
	int	CmdFlags(void) const;
	
	static Command* GlobalCommands(void);
	static Command* ResolveCommand(const char* str);
};

#define REGISTER_COMMAND(IDENTIFIER) extern Command IDENTIFIER;


REGISTER_COMMAND(g_cmd_help);
REGISTER_COMMAND(g_cmd_tree);
REGISTER_COMMAND(g_cmd_symbols);

REGISTER_COMMAND(g_cmd_watch);

REGISTER_COMMAND(g_cmd_exit);
REGISTER_COMMAND(g_cmd_cache_list);
REGISTER_COMMAND(g_cmd_cache_update);
REGISTER_COMMAND(g_cmd_cache_remove);
REGISTER_COMMAND(g_cmd_cache_clear);

#undef REGISTER_COMMAND
