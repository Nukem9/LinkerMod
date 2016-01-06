#pragma once

#include "files.h"
#include "miniz.h"

int __cdecl IWD_IWDHandler(const char* iwdPath, const char* iwdName);
int __cdecl IWD_IWDExtract(const char* iwdPath, const char* iwdName);