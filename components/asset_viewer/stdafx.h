#pragma once

#define _CRT_SECURE_NO_WARNINGS 1

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <intrin.h>
#include <shellapi.h>

//
// Shared files
//
#include "../shared/utility.h"

#include "../shared/minidx9/Include/d3dx9.h"
#pragma comment(lib, "../shared/minidx9/Lib/x86/d3dx9.lib")
//
// 
//

#define SRCLINE(x)
#define CHECK_SIZE(Type, Size) static_assert(sizeof(Type) == Size, "Invalid type size!");
