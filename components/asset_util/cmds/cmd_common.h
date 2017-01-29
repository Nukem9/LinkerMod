#pragma once

#include "../common/io.h"

int Cmd_Help_f(int argc, char** argv);
int Cmd_Ents_f(int argc, char** argv);

int Cmd_Extract_FF_f(int argc, char** argv);
int Cmd_Extract_IWD_f(int argc, char** argv);

int Cmd_BspInfo_f(int argc, char** argv);

int Cmd_CSVGen_f(int argc, char** argv);

//
// An empty test command used for debugging
//
static int Cmd_Test_f(int argc, char** argv)
{
	return 0;
}
