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
	{ "", NULL, 0 },
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
