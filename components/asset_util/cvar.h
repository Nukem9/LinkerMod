#pragma once
#include "arg.h"
#include <Windows.h>

//
// The maximum number of global cvars
//
#define GLOBAL_CVAR_MAX 32
#define CVAR_STRLEN MAX_PATH

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
	float	float_val;
	char	str_val[CVAR_STRLEN];
	
	friend class GCVar;
	
public:
	CVar(void);

	CVar(const char* name, char arg_shortcut, const char* description, int defaultValue);
	CVar(const char* name, char arg_shortcut, const char* description, bool defaultValue);
	CVar(const char* name, char arg_shortcut, const char* description, float defaultValue);
	CVar(const char* name, char arg_shortcut, const char* description, const char* defaultValue);

	~CVar(void);

	CVAR_TYPE Type(void) const;

	//
	// Currently only supports bool cvars
	//
	bool		Enable(void);
	bool		Disable(void);
	bool		Toggle(void);

	int			AssignValue(bool value);
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

	static void	DumpList(void);

	static CVar* ResolveCVar(const char* str);
};

#define REGISTER_GLOBAL_CVAR(IDENTIFIER) extern CVar IDENTIFIER;

REGISTER_GLOBAL_CVAR(g_verbose);
REGISTER_GLOBAL_CVAR(g_logfile);
#if _DEBUG
REGISTER_GLOBAL_CVAR(g_dumpCVars);
#endif

#undef REGISTER_GLOBAL_CVAR

//
// Temporary Definitions for Use During Transition
//
#define ARG_FLAG_AUDIO		0
#define ARG_FLAG_CONVERT	0
#define ARG_FLAG_EVERY		0
#define ARG_FLAG_IMAGE		0
#define ARG_FLAG_SND		0
#define ARG_FLAG_FF			0
#define ARG_FLAG_LOCALIZED	0
#define ARG_FLAG_OVERWRITE	0
#define ARG_FLAG_SETUP		0
