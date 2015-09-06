#pragma once

#include <fstream>

typedef int  __cdecl fclose_t(FILE *File);
static fclose_t* o_fclose = (fclose_t*)0x004F3C80;

void mfh_fcloseDeps();