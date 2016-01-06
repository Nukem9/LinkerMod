#pragma once

extern bool ARG_FLAG_AUDIO; // -a
extern bool ARG_FLAG_IMAGE; // -i
extern bool ARG_FLAG_OVERWRITE; // -o
extern bool ARG_FLAG_SETUP; // -s

void Arg_PrintUsage(void);
int Arg_HandleArguments(int argc, char** argv);