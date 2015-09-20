#pragma once

typedef int(__cdecl * arg_f)();

struct arg_t
{
	char* key;
	char* desc;
	arg_f func;
};

extern arg_t* g_Args;
extern int g_HDR;

void PatchArguments();