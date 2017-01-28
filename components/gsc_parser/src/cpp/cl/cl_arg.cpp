#include <stdlib.h>
#include <ctype.h>

#include "cl_arg.h"
#include <string.h>
#include "cl_cvar.h"
#include "cl_cmd.h"
#include "../util/llist.h"
#include "../sys/sys_platform.h"

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
			"Example:",	APPLICATION_NAME" --tree -f 'maps/utility.gsc'");
	
	printf("Options:\n");
	for(int i = 0; i < 255; i++)
	{
		if(g_shortcut[i] && g_shortcut[i]->Flags() & (ARG_GLOBAL | ARG_CVAR))
		{
			printf("  -%c, --%-22s%s\n", i, g_shortcut[i]->Name(), g_shortcut[i]->Description());
		}
	}
	printf("\n");
	
	printf("Launch Commands:\n");
	for(Command* cmd = Command::GlobalCommands(); cmd; cmd = cmd->NextElem())
	{
		if(cmd->CmdFlags() & COMMAND_LAUNCH)
		{
			printf("  %-22s%s\n", cmd->Name(), cmd->Description());
		}
	}
	printf("\n");
	
	printf("Watch Commands:\n");
	for(Command* cmd = Command::GlobalCommands(); cmd; cmd = cmd->NextElem())
	{
		if(cmd->CmdFlags() & COMMAND_WATCH)
		{
			printf("  %-22s%s\n", cmd->Name(), cmd->Description());
		}
	}
	printf("\n");
}

int Arg_ParseArgument(char*** consumable_argv, int* consumable_argc)
{
	char**& argv = *consumable_argv;
	int& argc = *consumable_argc;
	
	const char* argStr = argv[0];

	int len = strlen(argStr);
	if(!len)
	{
		fprintf(stderr, "Error: Zero length argument\n");
		return 1;
	}
	
	if(len >= 2 && (argStr[0] == '-' && argStr[1] == '-'))
	{
		printf("Full name found\n");
		return 0;
	}

	if(len == 2 && argStr[0] == '-')
	{
		CVar* cvar = (CVar*)g_shortcut[(int)argStr[1]];
		if(!cvar)
		{
			fprintf(stderr, "Error: Unrecognized argument '%s'\n", argStr);
			return 1;
		}
		
		if(cvar->Flags() & (ARG_CVAR | ARG_GLOBAL))
		{
			if(argc < 2)
			{
				fprintf(stderr, "Error: No value provided for argument '%s'\n", argStr);
				return 3;
			}
			
			cvar->AssignRawString(argv[1]);
			argc-=2;
			argv+=2;
			return 0;
		}
	}
	
	//
	// No arguments were consumed - print error & abort oncoming infinite loop
	//
	fprintf(stderr, "Error: Unrecognized argument '%s'\n", argStr);
	return 1;
}

int Arg_ParseArguments(int argc, char** argv, ArgParsedInfo* out_info)
{
	if(argc < 1)
	{
		return -1;
	}
	
	out_info->cmd = Command::ResolveCommand(*argv);
	if(!out_info->cmd)
	{
		fprintf(stderr, "Error: Command '%s' not recognized\n", *argv);
		return 2;
	}
	
	out_info->argc = argc;
	out_info->argv = new char*[argc];
	for(int i = 0; i < argc; i++)
	{
		out_info->argv[i] = strdup(argv[i]);
	} 
	
	/*char** consumable_argv = &argv[1];
	for(int consumable_argc = argc - 1; consumable_argc; )
	{
		if(int err = Arg_ParseArgument(&consumable_argv, &consumable_argc))
		{
			return err;
		}
	}*/
	
	return 0;
}

int Arg_ParseCmdLine(char* cmdLine, ArgParsedInfo* out_info)
{
	char* cl = strdup(cmdLine);
	int cl_len = strlen(cl);
	
	int argc = 0;
	
	char* arg_list[32];
	memset(arg_list, 0, sizeof(char*) * 32);
	
	bool last_was_space = true; //skips leading spaces
	for(int i = 0; i < cl_len; i++)
	{
		switch(cl[i])
		{
			case '\\':
				i++;
				last_was_space = false;
				continue;
			
			case '"':
			case '\'':
				for(int j = i+1; j < cl_len; j++)
				{
					if(cl[j] == cl[i] && cl[j-1] != '\\')
					{
						i = j;
					}
				}
				
				last_was_space = false;
				continue;
				
			default:
				int is_space = isspace(cl[i]);
				if(last_was_space && !is_space)
				{
					last_was_space = false;
					arg_list[argc++] = &cl[i];
					if(argc >= 32)
					{
						i = cl_len;
						continue;
					}
				}
				
				if((last_was_space = !!is_space))
				{
					cl[i] = '\0';
				}
				
				continue;
		}
	}
	
	/*printf("Argc: %d\n", argc);
	for(int i = 0; i < argc; i++)
	{
		printf("[%d]: %s\n", i, arg_list[i]);
	}*/
	
	int result = -1;
	
	if(argc)
	{
		result = Arg_ParseArguments(argc, arg_list, out_info);
	}

	free(cl);
	return result;
}
