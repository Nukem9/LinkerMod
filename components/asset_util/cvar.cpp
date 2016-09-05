#include "cvar.h"
#include "platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

CVar* g_cvar[GLOBAL_CVAR_MAX];

int g_cvar_count = 0;

//
// Used by the registration macro to register global cvars with g_cvar
//
class GCVar
{
public:
	GCVar(CVar* cvar)
	{
		g_cvar[g_cvar_count++] = cvar;
		cvar->flags |= ARG_GLOBAL;
	}
	
	~GCVar(void) { }
};

#define REGISTER_GLOBAL_CVAR(IDENTIFIER, NAME, SHORTCUT, DESCRIPTION, VALUE) CVar IDENTIFIER (NAME, SHORTCUT, DESCRIPTION, VALUE); GCVar gcv_##IDENTIFIER ( &IDENTIFIER );

REGISTER_GLOBAL_CVAR(g_verbose, "verbose", 'v', "Enable verbose logging", false);
REGISTER_GLOBAL_CVAR(g_logfile, "logfile", 'l', "Enable logging to file", false);
REGISTER_GLOBAL_CVAR(g_dumpCVars, "dump_cvars", 'd', "Print the cvar values to the console", false);

#undef REGISTER_GLOBAL_CVAR

CVar::CVar(void)
{
	memset(this, 0, sizeof(CVar));
	this->type = CVAR_NULL;
	this->flags = ARG_CVAR;
}

CVar::CVar(const char* name, char shortcut, const char* description, int defaultValue) : int_val(defaultValue)
{
	this->type = CVAR_INT;
	this->flags = ARG_CVAR;
	
	this->name = name;
	this->desc = description;
	
	if(shortcut)
	{
		this->shortcut = 0;
		this->RegisterShortcut(shortcut);
	}
	
	this->bool_val = (int_val != 0);
	snprintf(str_val, 32, "%d", int_val);
	this->float_val = (float)int_val;
}

CVar::CVar(const char* name, char shortcut, const char* description, bool defaultValue) : bool_val(defaultValue)
{
	this->type = CVAR_BOOL;
	this->flags = ARG_CVAR;
	
	this->name = name;
	this->desc = description;
	
	//
	// Allow all booleans to implicitly toggle when used with no value argument (or in a group)
	//
	this->flags |= ARG_IMPLICIT_VALUE;
	
	if(shortcut)
	{
		this->shortcut = 0;
		this->RegisterShortcut(shortcut);
	}
	
	this->int_val = bool_val ? 1 : 0;
	snprintf(str_val, 32, "%s", bool_val ? "true" : "false");
	this->float_val = (float)int_val;
}

CVar::CVar(const char* name, char shortcut, const char* description, float defaultValue) : float_val(defaultValue)
{
	this->type = CVAR_FLOAT;
	this->flags = ARG_CVAR;
	
	this->name = name;
	this->desc = description;
	
	if(shortcut)
	{
		this->shortcut = 0;
		this->RegisterShortcut(shortcut);
	}
	
	this->int_val = (int)float_val;
	this->bool_val = (int_val != 0);
	snprintf(str_val, 32, "%f", float_val);
}

CVar::CVar(const char* name, char shortcut, const char* description, const char* defaultValue)
{
	this->type = CVAR_STRING;
	this->flags = ARG_CVAR;
	
	this->name = name;
	this->desc = description;
	
	if(shortcut)
	{
		this->shortcut = 0;
		this->RegisterShortcut(shortcut);
	}
	
	strncpy(this->str_val, defaultValue, 31);
	this->str_val[31] = '\0';
	
	this->bool_val = (str_val[0] != 0);
	this->int_val = bool_val ? 1 : 0;
	this->float_val = (float)this->int_val;
}


CVar::~CVar(void)
{
}

/*bool CVar::Enable(void)
{
	switch(this->type)
	{
		case CVAR_BOOL:
			if(stricmp(this->str_val, "false") == 0)
				this->bool_val = false;
			else if (stricmp(this->str_val, "true") == 0)
				this->bool_val = true;
			else
				this->bool_val = (bool)atoi(str_val);
			this->int_val = bool_val ? 1 : 0;
			this->float_val = (float)int_val;
			break;
		case CVAR_INT:
			this->int_val = atoi(str_val);
			this->bool_val = (int_val != 0);
			this->float_val = (float)int_val;
			break;
		case CVAR_FLOAT:
			this->float_val = (float)atof(str_val);
			this->int_val = (int)float_val;
			this->bool_val = (int_val != 0);
			break;
		case CVAR_STRING:
			this->bool_val = (str_val[0] != 0);
			this->int_val = bool_val ? 1 : 0;
			this->float_val = (float)this->int_val;
			break;
		default:
			return 1;
	}
}

bool CVar::Disable(void)
{
	this->bool_val = false;
	this->int_val = 0;
	this->float_val = 0.0f;
	this->str_val = "NULL";
}*/

//
// Assign raw command line string data
// automatically convert to the proper data type
//
int CVar::AssignRawString(const char* val)
{
	strncpy(this->str_val, val, 31);
	this->str_val[31] = '\0';
	
	switch(this->type)
	{
		case CVAR_BOOL:
			if(stricmp(this->str_val, "false") == 0)
				this->bool_val = false;
			else if (stricmp(this->str_val, "true") == 0)
				this->bool_val = true;
			else
				this->bool_val = (atoi(str_val) != 0);
			this->int_val = bool_val ? 1 : 0;
			this->float_val = (float)int_val;
			break;
		case CVAR_INT:
			this->int_val = atoi(str_val);
			this->bool_val = (int_val != 0);
			this->float_val = (float)int_val;
			break;
		case CVAR_FLOAT:
			this->float_val = (float)atof(str_val);
			this->int_val = (int)float_val;
			this->bool_val = (int_val != 0);
			break;
		case CVAR_STRING:
			this->bool_val = (str_val[0] != 0);
			this->int_val = bool_val ? 1 : 0;
			this->float_val = (float)this->int_val;
			break;
		default:
			return 1;
	}
	
	return 0;
}

int	CVar::ValueInt(void) const
{
	return this->int_val;
}

bool CVar::ValueBool(void) const
{
	return this->bool_val;
}

float CVar::ValueFloat(void) const
{
	return this->float_val;
}

const char*	CVar::ValueString(void) const
{
	return this->str_val;
}

void CVar_DumpCVars(void)
{
	for(int i = 0; i < GLOBAL_CVAR_MAX; i++)
	{
		if(CVar* cvar = g_cvar[i])
		{
			if(strlen(cvar->Name()) <= 16)
			{
				printf("  %-16s : %-22s\n", cvar->Name(), cvar->ValueString());	
			}
			else
			{
				printf("  %-32s : %-22s\n", cvar->Name(), cvar->ValueString());
			}
		}
	}
}
