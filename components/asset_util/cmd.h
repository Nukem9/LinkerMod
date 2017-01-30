#pragma once
#include "arg.h"
#include "cvar.h"
#include "common/llist.h"

typedef int (*cmd_func_t)(int argc, char** argv);

class Command : public Argument, public LList<Command>
{
private:
	static Command* g_cmds;
	cmd_func_t func;
	
	//
	// A null terminated array of CVar pointers that represents the
	// non-global CVars that can be used with this command
	//
	CVar** const cvars;

public:
	Command(const char* name, const char* description, cmd_func_t func, CVar** const cvars = NULL);
	
	int Exec(int argc, char** argv) const;
	CVar** const CVars(void) const;
	void DumpLocalCVars(void) const;
	
	static Command* GlobalCommands(void);
	static Command* ResolveCommand(const char* str);
};

#define REGISTER_GLOBAL_COMMAND(IDENTIFIER) extern Command IDENTIFIER;

REGISTER_GLOBAL_COMMAND(g_cmd_help);
REGISTER_GLOBAL_COMMAND(g_cmd_ents);
REGISTER_GLOBAL_COMMAND(g_cmd_extract_ff);
REGISTER_GLOBAL_COMMAND(g_cmd_extract_iwd);

REGISTER_GLOBAL_COMMAND(g_cmd_bsp_info);

REGISTER_GLOBAL_COMMAND(g_cmd_csvgen);

#undef REGISTER_GLOBAL_COMMAND
