#pragma once

extern bool ARG_FLAG_AUDIO;		// -a
extern bool ARG_FLAG_CONVERT;	// -c
extern bool ARG_FLAG_EVERY;		// -e
extern bool ARG_FLAG_IMAGE;		// -i
extern bool ARG_FLAG_SND;		// -n
extern bool ARG_FLAG_FF;		// -r
extern bool ARG_FLAG_LOCALIZED;	// -l
extern bool ARG_FLAG_OVERWRITE;	// -o
extern bool ARG_FLAG_SETUP;		// -s
extern bool ARG_FLAG_VERBOSE;	// -v

void Arg_PrintUsage(void);
int Arg_HandleArguments(int argc, char** argv);
