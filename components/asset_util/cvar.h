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

	//
	// Dump all global CVars and their values
	//
	static void	DumpCVars(void);

	//
	// Dump all cvars in the given null-terminated list and their values
	//
	static void	DumpCVars(CVar** const cvars);

	//
	// Attempts to resolve a cvar from a given argument string
	// localCVars (NULL terminated) has higher priority than the global CVars
	// Returns NULL if there is no match
	//
	static CVar* ResolveCVar(const char* str, CVar** localCVars = NULL);
};

#define REGISTER_CVAR(IDENTIFIER) extern CVar IDENTIFIER;
#define REGISTER_GLOBAL_CVAR(IDENTIFIER) REGISTER_CVAR(IDENTIFIER)

//
// Register Global CVars
//
REGISTER_GLOBAL_CVAR(g_verbose);
REGISTER_GLOBAL_CVAR(g_logfile);
REGISTER_GLOBAL_CVAR(fs_overwrite);

#if _DEBUG
REGISTER_GLOBAL_CVAR(g_dumpCVars);
#endif

//
// Register Standard CVars
//
REGISTER_CVAR(g_var);
REGISTER_CVAR(g_extractAll);
REGISTER_CVAR(g_extractImages);
REGISTER_CVAR(g_extractSounds);
REGISTER_CVAR(g_useLocalized);

REGISTER_CVAR(fs_outdir);

#undef REGISTER_GLOBAL_CVAR
#undef REGISTER_CVAR
