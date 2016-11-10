#pragma once

#define _CRT_SECURE_NO_WARNINGS 1

#pragma comment(lib, "detours.lib")
#include "../shared/detours/Detours.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <intrin.h>
#include <shellapi.h>

//
// Shared files
//
#include "../shared/shared_utility.h"

#include "../shared/minidx9/Include/d3dx9.h"
#pragma comment(lib, "../shared/minidx9/Lib/x86/d3dx9.lib")

//
// Nvidia Nsight patches
//
#include "nvidia.h"

#define SRCLINE(x)
#define CHECK_SIZE(Type, Size) static_assert(sizeof(Type) == Size, "Invalid type size!");

//
// Asset Viewer Mod Options
// Can be used to easily toggle certain tweaks
//
#define ASSET_VIEWER_DISABLE_MATERIAL_ASSERT 1

