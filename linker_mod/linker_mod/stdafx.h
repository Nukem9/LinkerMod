#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <intrin.h>

#pragma comment(lib, "detours/Detours.lib")
#include "detours/Typedef.h"
#include "detours/Detours.h"

void assert(const char *File, int Line, bool Condition, const char *Format, ...);

#include "T6.h"
#include "crc32.h"