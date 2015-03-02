#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <intrin.h>
#include <Psapi.h>

#pragma comment(lib, "detours/Detours.lib")
#include "detours/Typedef.h"
#include "detours/Detours.h"

#include "CEG.h"

#include "common.h"
#include "dvar.h"
#include "sv_ccmds_mp.h"
#include "cl_main_mp.h"
#include "cl_console.h"