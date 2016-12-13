#include <stdlib.h>
#include "arg.h"
#include "string.h"
#include "cvar.h"
#include "cmd.h"
#include "common/llist.h"
#include "platform.h"
#include "common/io.h"
#include "shared_assert.h"

Argument* g_shortcut[255] = { NULL };

int Argument::RegisterShortcut(char shortcut)
{
	if (this->shortcut)
	{
		Con_Error("Error: Could not register shortcut '-%c' for argument '%s' - arg already has shortcut '-%c'\n", shortcut, this->name, this->shortcut);
		return 3;
	}

	if (shortcut)
	{
		if (!g_shortcut[(int)shortcut])
		{
			this->shortcut = shortcut;
			g_shortcut[(int)shortcut] = this;
			return 0;
		}
		else
		{
			Con_Error("Error: Could not initialize argument '%s' - shortcut '-%c' already exists for another argument\n", name, shortcut);
			return 1;
		}
	}

	Con_Error("Error: Could not register shortcut'-%c' for argument '%s'\n", shortcut, this->name);
	return 2;
}

const char*  Argument::Name(void) const
{
	return this->name;
}

const char*  Argument::Description(void) const
{
	return this->desc;
}

int  Argument::Flags(void) const
{
	return this->flags;
}

ArgParsedInfo::ArgParsedInfo(void) : cmd(NULL), argc(0), argv(NULL)
{
}

ArgParsedInfo::~ArgParsedInfo(void)
{
	for (; argc;)
	{
		free((void*)this->argv[--argc]);
	}

	delete[] this->argv;
	this->argv = NULL;
}

Command* ArgParsedInfo::Cmd(void) const
{
	return this->cmd;
}

int ArgParsedInfo::Argc(void) const
{
	return this->argc;
}

char** ArgParsedInfo::Argv(void) const
{
	return this->argv;
}

char* ArgParsedInfo::Argv(int index) const
{
	if (index >= argc || index < 0)
	{
		return (char*)"";
	}

	return this->argv[index];
}

static void Arg_PrintUsage_GlobalOptions(void)
{
	Con_Print("Global Options:\n");
	for (int i = 0; i < 255; i++)
	{
		//
		// Currently hides any cvars that do not have a shortcut
		//
		if (g_shortcut[i] && g_shortcut[i]->Flags() & (ARG_GLOBAL | ARG_CVAR))
		{
			Con_Print("  -%c, --%-22s%s\n", i, g_shortcut[i]->Name(), g_shortcut[i]->Description());
		}
	}
	Con_Print("\n");
}

static void Arg_PrintUsage_CommandCVars(Command* cmd)
{
	if (cmd->CVars() == NULL)
		return;

	Con_Print("Command Options:\n");
	bool empty = true;
	for (int i = 0; cmd->CVars()[i]; i++)
	{
		Con_Print("  --%-22s%s\n", cmd->CVars()[i]->Name(), cmd->CVars()[i]->Description());
		empty = false;
	}

	if (empty)
	{
		Con_Print("  %-22s\n", "None (use global options)");
	}

	Con_Print("\n");
}

void Arg_PrintUsage(void)
{
	Con_Print("%-9s%s\n%-9s%s\n\n",
		"Usage:", APPLICATION_NAME" [command] [options]",
		"Example:", APPLICATION_NAME" ents -v 'zone/Common/mp_cairo.ff'");

	Arg_PrintUsage_GlobalOptions();

	Con_Print("Commands:\n");
	for (Command* cmd = Command::GlobalCommands(); cmd; cmd = cmd->NextElem())
	{
		Con_Print("  %-22s%s\n", cmd->Name(), cmd->Description());
	}
	Con_Print("\n");
}

void Arg_PrintUsage(Command* cmd)
{
	ASSERT(cmd != NULL);

	Con_Print("%-9s%s %s %s\n\n",
		"Usage:", APPLICATION_NAME, cmd->Name(), "[options]");

	Arg_PrintUsage_GlobalOptions();
	Arg_PrintUsage_CommandCVars(cmd);
}

int Arg_ParseArgument(char*** consumable_argv, int* consumable_argc, CVar** const cmdCVars = NULL)
{
	char**& argv = *consumable_argv;
	int& argc = *consumable_argc;

	const char* argStr = argv[0];

	int len = strlen(argStr);
	if ((len == 0) ||
		(len == 1 && (argStr[0] == '-')) ||
		(len == 2 && (argStr[0] == '-' && argStr[1] == '-')))
	{
		Con_Error("Error: Zero length argument\n");
		return 1;
	}

	CVar* cvar = NULL;
	if (len > 2 && (argStr[0] == '-' && argStr[1] == '-'))
	{
		cvar = CVar::ResolveCVar(argStr + 2, cmdCVars);
	}
	else if (len >= 2 && argStr[0] == '-')
	{
		int count = 0;
		for (const char* c = argStr + 1; *c; c++)
		{
			cvar = (CVar*)g_shortcut[*c];
			if (cvar == NULL)
			{
				Con_Warning("Warning: Unrecognized flag '%c' in argument '%s'\n", *c, argStr);
				continue;
			}

			if (cvar->Flags() & ARG_IMPLICIT_VALUE && cvar->Type() == CVAR_BOOL)
			{
				cvar->Toggle();
				count++;
			}
		}
		
		if (count == 0)
		{
			Con_Error("Error: Implicit CVar argument group '%s' is completely unrecognized\n", argStr);
			return 2;
		}

		**argv++ = '\0';
		argc--;
		return 0;
	}
	else // A non CVar argument was used, ignore it and pass it on to the command
	{
		argc--;
		argv++;
		return 0;
	}

	//
	// An recognized CVar was used (It was probably an argument meant for the command)
	//	- print verbosemode warning & consume the arg anyway
	//
	if (cvar == NULL)
	{
		Con_Warning_v("Warning: Unrecognized argument '%s'\n", argStr);
		argc--;
		argv++;
		return 0;
	}


	//
	// Automatically toggle implicit bools
	//
	if (cvar->Flags() & ARG_IMPLICIT_VALUE && cvar->Type() == CVAR_BOOL)
	{
		cvar->Toggle();
		argc--;
		**argv++ = '\0'; // Prevent the cvar argument from being passed to the command
		return 0;
	}

	if (argc < 2)
	{
		Con_Error("Error: No value provided for explicit argument '%s'\n", argStr);
		return 3;
	}

	//
	// Consume both the cvar name and the value string
	//
	cvar->AssignRawString(argv[1]);
	**argv++ = '\0'; // Prevent the cvar argument from being passed to the command
	**argv++ = '\0';
	argc -= 2;
	return 0;


	//
	// No arguments were consumed - print error & abort oncoming infinite loop
	//
	Con_Error("Error: Unrecognized argument '%s'\n", argStr);
	return 1;
}

int Arg_ParseArguments(int argc, char** argv, ArgParsedInfo* out_info)
{
	out_info->cmd = Command::ResolveCommand(*argv);
	if (!out_info->cmd)
	{
		Con_Error("Error: Command '%s' not recognized\n", *argv);
		return 2;
	}

	//
	// CVar Support
	// Extract the CVar data from the arg list before passing the rest of the args to the command
	//
	char** consumable_argv = &argv[1];
	for (int consumable_argc = argc - 1; consumable_argc; /*automatically decremented by Arg_ParseArgument*/)
	{
		if (int err = Arg_ParseArgument(&consumable_argv, &consumable_argc, out_info->cmd->CVars()))
		{
			return err;
		}
	}

	out_info->argc = 0;
	out_info->argv = new char*[argc];
	for (int i = 0; i < argc; i++)
	{
		if (argv[i][0] == '\0')
			continue;

		out_info->argv[out_info->argc] = strdup(argv[i]);
		out_info->argc++;

	}

	return 0;
}