#pragma once

#include "../common/io.h"
#include "../cvar.h"

int Cmd_Default_f(int argc, char** argv);
int Cmd_Help_f(int argc, char** argv);
int Cmd_Ents_f(int argc, char** argv);

//
// An empty test command used for debugging
//
static int Cmd_Test_f(int argc, char** argv)
{
	return 0;
}
