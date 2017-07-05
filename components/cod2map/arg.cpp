#include "stdafx.h"

#define VANILLA_ARG_COUNT 29

struct Argument
{
	const char *arg;
	const char *description;

	// Returns the number of args it consumes
	int(__cdecl *func)(int argc, const char **argv);
};

Argument* g_args = (Argument*)0x004F4390;

Argument g_customArgs[] = 
{
	{ "-debugProbes", "Add a reflective debug sphere on top of each reflection probe", &Probe_EnableDebugSpheres_f }
};

void __declspec(naked) Arg_Handle_hk(void)
{
	_asm
	{
		push ebx // argv
		push [esp+8] // argc
		call Arg_Handle
		add esp, 8
		retn
	}
}

int Arg_CheckForMatch(const char* name, int index)
{
	if (index < VANILLA_ARG_COUNT)
		return _stricmp(name, g_args[index].arg);

	return _stricmp(name, g_customArgs[index - VANILLA_ARG_COUNT].arg);
}

int Arg_Handle(int argc, const char** argv)
{
	int argIndex = 0;
	while (Arg_CheckForMatch(*argv, argIndex))
	{
		if (++argIndex < VANILLA_ARG_COUNT + ARRAYSIZE(g_customArgs))
			continue;

		// There was an unknown arg - print usage
		Com_Printf("\n\nUnknown argument '%s'\n\n", *argv);
		Com_Printf("USAGE: cod2map [options] mapname, where options are 0 or more of the following.\n");
		Com_Printf("Options ignore capitalization; it is only present in the list for clarity.\n");

		// Print vanilla args
		for (int i = 0; i < VANILLA_ARG_COUNT; i++)
		{
			Com_Printf("%-20s %s\n", g_args[i].arg, g_args[i].description);
		}

		// Print custom args
		for (int i = 0; i < ARRAYSIZE(g_customArgs); i++)
		{
			Com_Printf("%-20s %s\n", g_customArgs[i].arg, g_customArgs[i].description);
		}

		exit(-1);
	}

	if (argIndex < VANILLA_ARG_COUNT)
		return g_args[argIndex].func(argc, argv);
	else
		return g_customArgs[argIndex - VANILLA_ARG_COUNT].func(argc, argv);
}
