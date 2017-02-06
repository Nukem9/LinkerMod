#include "cmd.h"
#include "cvar.h"
#include <stdio.h>
#include <string.h>

#include "cmds/cmd_common.h"
#include "common\io.h"

#define REGISTER_GLOBAL_COMMAND(IDENTIFIER, NAME, DESCRIPTION, FUNC, CVARS) CVar* IDENTIFIER##_cvarlist[] = CVARS; Command IDENTIFIER(NAME, DESCRIPTION, FUNC, IDENTIFIER##_cvarlist);
#define CMD_CVARS(...) { __VA_ARGS__ , NULL }
#define CMD_GLOBALCVARS { NULL }

Command* Command::g_cmds = NULL;

#if _DEBUG
REGISTER_GLOBAL_COMMAND(g_cmd_test, "test", "An empty test command", Cmd_Test_f, CMD_CVARS(&g_var));
#endif
REGISTER_GLOBAL_COMMAND(g_cmd_default, "", "(Default command for when no explicit command is given)", Cmd_Default_f, CMD_GLOBALCVARS);
REGISTER_GLOBAL_COMMAND(g_cmd_help, "help", "Print usage information", Cmd_Help_f, CMD_GLOBALCVARS);


#undef CMD_GLOBALCVARS
#undef CMD_CVARS
#undef REGISTER_GLOBAL_COMMAND

Command::Command(const char* name, const char* description, cmd_func_t func, CVar** const cvars) : func(func), cvars(cvars)
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

int Command::Exec(int argc, char** argv) const
{
#if _DEBUG
	const char* command = this->name;
	if (*command == '\0')
		command = "$default";

	Con_Printf("Executing command: '%s' with the following arguments:\n", command);
	for(int i = 0; i < argc; i++)
	{
		Con_Printf("  [%d] %s\n", i, argv[i]);
	}
	
	Con_Printf("\n");
#endif

	return this->func(argc, argv);
}

CVar** const Command::CVars(void) const
{
	return this->cvars;
}

void Command::DumpLocalCVars(void) const
{
	CVar::DumpCVars(this->cvars);
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
