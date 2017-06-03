#pragma once

#pragma comment(lib, "detours.lib")
#include "../shared/detours/Detours.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <intrin.h>

//
// Shared files
//
#include "../shared/shared_utility.h"
#include "../shared/shared_version.h"

#include "linker.h"

#include "T5.h"
#include "crc32.h"
#include "assertive.h"