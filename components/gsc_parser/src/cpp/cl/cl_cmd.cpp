#include "cl_cmd.h"
#include <stdio.h>
#include <string.h>

#include "commands/cmd_common.h"

#define REGISTER_COMMAND(IDENTIFIER, NAME, DESCRIPTION, FUNC, FLAGS) Command IDENTIFIER(NAME, DESCRIPTION, FUNC, FLAGS);

Command* Command::g_cmds = NULL;

REGISTER_COMMAND(g_cmd_help, "help", "Print usage information", Cmd_Help_f, COMMAND_LAUNCH | COMMAND_WATCH);
REGISTER_COMMAND(g_cmd_tree, "tree", "Print the AST for a given script file", Cmd_Tree_f, COMMAND_LAUNCH | COMMAND_WATCH);
REGISTER_COMMAND(g_cmd_symbols, "symbols", "Print the top level symbols for a given script file", Cmd_Symbols_f, COMMAND_LAUNCH | COMMAND_WATCH);

REGISTER_COMMAND(g_cmd_watch, "watch", "Start watch/reentrant mode", Cmd_Watch_f, COMMAND_LAUNCH);

REGISTER_COMMAND(g_cmd_exit, "exit", "Close the parser", Cmd_Exit_f, COMMAND_WATCH);
REGISTER_COMMAND(g_cmd_cache_list, "cache-list", "Start watch/reentrant mode", Cmd_Cache_List_f, COMMAND_WATCH);
REGISTER_COMMAND(g_cmd_cache_update, "cache-update", "Add or Update a cache entry", Cmd_Cache_Update_f, COMMAND_WATCH);
REGISTER_COMMAND(g_cmd_cache_remove, "cache-remove", "Remove a cache entry", Cmd_Cache_Remove_f, COMMAND_WATCH);
REGISTER_COMMAND(g_cmd_cache_clear, "cache-clear", "Remove all cache entries", Cmd_Cache_Clear_f, COMMAND_WATCH);

#undef REGISTER_COMMAND

Command::Command(const char* name, const char* description,  cmd_func_t func, int cmd_flags) : func(func), cmd_flags(cmd_flags)
{
	this->SetOwner(this);
	
	if(!Command::g_cmds)
	{
		g_cmds = this;
	}
	else
	{
		g_cmds->AddToEnd(this);
	}
	
	this->name = name;
	this->desc = description;
	this->flags = ARG_COMMAND;
}

int Command::CmdFlags(void) const
{
	return cmd_flags;
}

int Command::Exec(int argc, char** argv) const
{
#if _DEBUG
	printf("Executing command: '%s' with the following arguments:\n", this->name);
	for(int i = 0; i < argc; i++)
	{
		printf("  [%d] %s\n", i, argv[i]);
	}
	printf("\n");
#endif

	return this->func(argc, argv);
}

Command* Command::GlobalCommands(void)
{
	return Command::g_cmds;
}

//
// Attempts to resolve a command from a given argument string
// Returns NULL if there is no match
//
Command* Command::ResolveCommand(const char* str)
{
	for(Command* cmd = Command::g_cmds; cmd; cmd = cmd->NextElem())
	{
		if(strcmp(cmd->Name(), str) == 0)
		{
			return cmd;
		}
	}
	
	return NULL;
}
