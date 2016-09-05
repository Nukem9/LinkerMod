#include <stdlib.h>
#include "arg.h"
#include "string.h"
#include "cvar.h"
#include "cmd.h"
#include "common/llist.h"
#include "platform.h"

Argument* g_shortcut[255] = {NULL};

int Argument::RegisterShortcut(char shortcut)
{
	if(this->shortcut)
	{
		fprintf(stderr, "Error: Could not register shortcut '-%c' for argument '%s' - arg already has shortcut '-%c'\n", shortcut, this->name, this->shortcut);
		return 3;
	}
	
	if(shortcut)
	{
		if(!g_shortcut[(int)shortcut])
		{
			this->shortcut = shortcut;
			g_shortcut[(int)shortcut] = this;
			return 0;
		}
		else
		{
			fprintf(stderr, "Error: Could not initialize argument '%s' - shortcut '-%c' already exists for another argument\n", name, shortcut);
			return 1;
		}
	}
	
	fprintf(stderr, "Error: Could not register shortcut'-%c' for argument '%s'\n", shortcut, this->name);
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
	for(; argc; )
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
	if(index >= argc || index < 0)
	{
		return (char*)"";
	}
	
	return this->argv[index];
}

void Arg_PrintUsage(void)
{
	printf(	"%-9s%s\n%-9s%s\n\n",
			"Usage:",	APPLICATION_NAME" [command] [options]",
			"Example:",	APPLICATION_NAME" ents -v 'zone/Common/mp_cairo.ff'");
	
	printf("Options:\n");
	for(int i = 0; i < 255; i++)
	{
		if(g_shortcut[i] && g_shortcut[i]->Flags() & (ARG_GLOBAL | ARG_CVAR))
		{
			printf("  -%c, --%-22s%s\n", i, g_shortcut[i]->Name(), g_shortcut[i]->Description());
		}
	}
	printf("\n");
	
	printf("Commands:\n");
	for(Command* cmd = Command::GlobalCommands(); cmd; cmd = cmd->NextElem())
	{
		printf("  %-22s%s\n", cmd->Name(), cmd->Description());
	}
	printf("\n");
}

int Arg_ParseArgument(char*** consumable_argv, int* consumable_argc)
{
	char**& argv = *consumable_argv;
	int& argc = *consumable_argc;
	
	const char* argStr = argv[0];

	int len = strlen(argStr);
	if( (len == 0) || 
		(len == 1 && (argStr[0] == '-')) ||
		(len == 2 && (argStr[0] == '-' && argStr[1] == '-')) )
	{
		fprintf(stderr, "Error: Zero length argument\n");
		return 1;
	}

	CVar* cvar = NULL;
	if (len == 2 && argStr[0] == '-')
	{
		cvar = (CVar*)g_shortcut[(int)argStr[1]];
	}
	else if(len > 2 && (argStr[0] == '-' && argStr[1] == '-'))
	{
		cvar = CVar::ResolveCVar(argStr + 2);
	}
	
	//
	// An undefined CVar was used - print error & abort oncoming infinite loop
	//
	if (cvar == NULL)
	{
		fprintf(stderr, "Error: Unrecognized argument '%s'\n", argStr);
		return 1;
	}

	if (cvar->Flags() & (ARG_CVAR | ARG_GLOBAL))
	{
		//
		// Automatically toggle implicit bools
		//
		if (cvar->Flags() & ARG_IMPLICIT_VALUE && cvar->Type() == CVAR_BOOL)
		{
			cvar->Toggle();
			argc--;
			argv++;
			return 0;
		}

		if (argc < 2)
		{
			fprintf(stderr, "Error: No value provided for argument '%s'\n", argStr);
			return 3;
		}

		//
		// Consume both the cvar name and the value string
		//
		cvar->AssignRawString(argv[1]);
		argc -= 2;
		argv += 2;
		return 0;
	}

	//
	// No arguments were consumed - print error & abort oncoming infinite loop
	//
	fprintf(stderr, "Error: Unrecognized argument '%s'\n", argStr);
	return 1;
}

int Arg_ParseArguments(int argc, char** argv, ArgParsedInfo* out_info)
{
	out_info->cmd = Command::ResolveCommand(*argv);
	if(!out_info->cmd)
	{
		fprintf(stderr, "Error: Command '%s' not recognized\n", *argv);
		return 2;
	}
	
	//
	// CVar Support
	// Extract the CVar data from the arg list before passing the rest of the args to the command
	//
	char** consumable_argv = &argv[1];
	for (int consumable_argc = argc - 1; consumable_argc; /*automatically decremented by Arg_ParseArgument*/ )
	{
		if (int err = Arg_ParseArgument(&consumable_argv, &consumable_argc))
		{
			return err;
		}
	}

	out_info->argc = argc;
	out_info->argv = new char*[argc];
	for(int i = 0; i < argc; i++)
	{
		out_info->argv[i] = strdup(argv[i]);
	} 
	
	return 0;
}