#include "stdafx.h"

struct BuiltinMethodDef
{
	const char *actionString;
	void(__cdecl *actionFunc)(scr_entref_t);
	int type;
};

struct BuiltinFunctionDef
{
	const char *actionString;
	void(__cdecl *actionFunc)();
	int type;
};

BuiltinMethodDef g_customMethods[] =
{
	{ "", NULL, 0 },
};

BuiltinFunctionDef g_customFunctions[] =
{
	{ "userstat_get", &Scr_UserStat_Get, 0 },
	{ "userstat_set", &Scr_UserStat_Set, 0 },
};

Scr_Method_t Custom_GetMethod(const char** pName)
{
	for (int i = 0; i < ARRAYSIZE(g_customMethods); i++)
	{
		BuiltinMethodDef& method = g_customMethods[i];
		if (strcmp(*pName, method.actionString) == 0)
		{
			*pName = method.actionString;
			return method.actionFunc;
		}
	}

	return NULL;
}

Scr_Function_t Custom_GetFunction(const char** pName)
{
	for (int i = 0; i < ARRAYSIZE(g_customFunctions); i++)
	{
		BuiltinFunctionDef& func = g_customFunctions[i];
		if (strcmp(*pName, func.actionString) == 0)
		{
			*pName = func.actionString;
			return func.actionFunc;
		}
	}

	return NULL;
}

Scr_Method_t __cdecl Scr_GetMethod(const char **pName, int *type)
{
	Scr_Method_t method = NULL;
	*type = 0;

	if (method = Actor_GetMethod(pName))
		return method;

	if (method = Sentient_GetMethod(pName))
		return method;

	if (method = Player_GetMethod(pName))
		return method;

	if (method = ScriptEnt_GetMethod(pName))
		return method;

	if (method = ScriptVehicle_GetMethod(pName))
		return method;
	
	if (method = HudElem_GetMethod(pName))
		return method;

	if (method = Custom_GetMethod(pName))
		return method;

	return BuiltIn_GetMethod(pName, type);
}

Scr_Function_t __cdecl Scr_GetFunction(const char **pName, int *type)
{
	Scr_Function_t func = NULL;
	*type = 0;

	if (func = Sentient_GetFunction(pName))
		return func;

	if (func = Custom_GetFunction(pName))
		return func;

	return BuiltIn_GetFunction(pName, type);
}

////////////////////////////////////////////////////////

void Scr_UserStat_Get(void)
{
	int paramCount = Scr_GetNumParam(SCRIPTINSTANCE_SERVER);
	if (paramCount != 1)
	{
		Scr_Error("USAGE: UserStat_Get( <stat name>)\n", 0);
		return;
	}

	const char* key = Scr_GetString(0, SCRIPTINSTANCE_SERVER);
	auto entry = g_userStats.GetStat(key);
	entry.EmitScriptValue(SCRIPTINSTANCE_SERVER);
}

void Scr_UserStat_Set(void)
{
	int paramCount = Scr_GetNumParam(SCRIPTINSTANCE_SERVER);
	if (paramCount != 2)
	{
		Scr_Error("USAGE: UserStat_Set( <stat name>, <value> )\n", 0);
		return;
	}

	const char* key = Scr_GetString(0, SCRIPTINSTANCE_SERVER);
	int type = Scr_GetType(1, SCRIPTINSTANCE_SERVER);

	float vec[3];

	switch (type)
	{
	case SCR_VALUE_INT:
		g_userStats.SetStat(key, Scr_GetInt(1, SCRIPTINSTANCE_SERVER));
		break;
	case SCR_VALUE_FLOAT:
		g_userStats.SetStat(key, Scr_GetFloat(1, SCRIPTINSTANCE_SERVER));
		break;
	case SCR_VALUE_STRING:
		g_userStats.SetStat(key, Scr_GetString(1, SCRIPTINSTANCE_SERVER));
		break;
	case SCR_VALUE_VEC:
		Scr_GetVector(1, vec, SCRIPTINSTANCE_SERVER);
		g_userStats.SetStat(key, vec);
		break;
	default:
		Scr_Error("Invalid Argument", 0);
	}
}
