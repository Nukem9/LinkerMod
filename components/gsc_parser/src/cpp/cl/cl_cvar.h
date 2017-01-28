#pragma once
#include "cl_arg.h"

//
// The maximum number of global cvars
//
#define GLOBAL_CVAR_MAX 32

enum CVAR_TYPE
{
	CVAR_NULL,
	
	CVAR_BOOL,
	CVAR_INT,
	CVAR_FLOAT,
	CVAR_STRING,
};

class CVar : public Argument
{
private:
	CVAR_TYPE type;
	
	int		int_val;
	bool	bool_val;
	char	str_val[32];
	float	float_val;
	
	friend class GCVar;
	
public:
	CVar(void);

	CVar(const char* name, char arg_shortcut, const char* description, int defaultValue);
	CVar(const char* name, char arg_shortcut, const char* description, bool defaultValue);
	CVar(const char* name, char arg_shortcut, const char* description, float defaultValue);
	CVar(const char* name, char arg_shortcut, const char* description, const char* defaultValue);

	~CVar(void);

	//
	// Currently only supports bool cvars
	//
	//bool		Enable(void);
	//bool		Disable(void);

	//int			AssignValue(bool value);
	//int			AssignValue(int value);
	//int			AssignValue(float value);
	//int			AssignValue(const char* value);

	//
	// Assign raw command line string data
	// automatically convert to the proper data type
	//
	int			AssignRawString(const char* val);

	int			ValueInt(void) const;
	bool		ValueBool(void) const;
	float		ValueFloat(void) const;
	const char*	ValueString(void) const;
};

#define REGISTER_GLOBAL_CVAR(IDENTIFIER) extern CVar IDENTIFIER;

REGISTER_GLOBAL_CVAR(g_verbose);
REGISTER_GLOBAL_CVAR(g_logfile);
REGISTER_GLOBAL_CVAR(g_dumpCVars);

#undef REGISTER_GLOBAL_CVAR

void CVar_DumpCVars(void);
