#pragma once

#define _CRT_SECURE_NO_WARNINGS 1

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <intrin.h>

//
// Shared files
//
#pragma comment(lib, "detours.lib")
#include "../shared/detours/Detours.h"

#include "../shared/shared_utility.h"
#include "../shared/minidx9/Include/d3dx9.h"
#pragma comment(lib, "../shared/minidx9/Lib/x86/d3dx9.lib")
//
// 
//

#include "ddl_api.h"