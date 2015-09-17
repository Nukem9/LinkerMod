#include "stdafx.h"

arg_t* g_Args = (arg_t*)0x004613E8;
int g_HDR = 0;

int Argument_HandleInteger(char** argv, int argv_len, int* dest, int min, int max)
{
	if (argv_len < 2)
		Com_FatalError("ERROR: %s: missing argument\n", argv[1]);

	int val = atol(argv[1]);
	*dest = val;

	if (val >= min)
	{
		if (val > max)
		{
			Con_Printf("%s: clamping to %i\n", *argv, max);
			*dest = max;
		}
	}
	else
	{
		Con_Printf("%s: clamping to %i\n", *argv, min);
		*dest = min;
	}
	return 2;
}

//
// Function used to set the -HDR argument behavior and initialize g_HDR
// argv is a pointer to strings containing current arg and its value
// argv_len contains the size of the argv array
//
int __cdecl argHDR_f(int argv_len, char **argv)
{
	int result = Argument_HandleInteger(argv, argv_len, &g_HDR, 0, 3);

	if (g_HDR)
		PatchHDR_Lightmaps();

	return result;
}

void PatchArguments()
{
	arg_t c_Args[] =
	{
		{ "-HDR", "Enables HDR lightmaps and lightgrid", (arg_f)&argHDR_f },
	};

	arg_t* o_Args = g_Args;
	g_Args = new arg_t[23 + ARRAYSIZE(c_Args)];
	memcpy(g_Args, o_Args, sizeof(arg_t) * 23);

	for (int i = 0; i < ARRAYSIZE(c_Args); i++)
	{
		g_Args[23 + i].key = c_Args[i].key;
		g_Args[23 + i].desc = c_Args[i].desc;
		g_Args[23 + i].func = c_Args[i].func;
	}

	//
	// Patch argument array pointers
	//
	void* args_key = (void*)&g_Args[0].key;
	void* args_desc = (void*)&g_Args[0].desc;
	void* args_func = (void*)&g_Args[0].func;

	PatchMemory(0x00440438, (PBYTE)&args_key, 4);
	PatchMemory(0x00440432, (PBYTE)&args_desc, 4);
	PatchMemory(0x00440617, (PBYTE)&args_key, 4);
	PatchMemory(0x00440641, (PBYTE)&args_func, 4);
	PatchMemory(0x00440698, (PBYTE)&args_key, 4);
	PatchMemory(0x00440692, (PBYTE)&args_desc, 4);

	//
	// Patch the argument array counters
	//
	BYTE argListCount = 23 + ARRAYSIZE(c_Args);
	int argListSize = argListCount * sizeof(arg_t);

	PatchMemory(0x00440450, (PBYTE)&argListSize, 4);
	PatchMemory(0x00440633, (PBYTE)&argListSize, 4);
	PatchMemory(0x004406B0, (PBYTE)&argListSize, 4);

	PatchMemory(0x00440657, (PBYTE)&argListCount, 1);
}