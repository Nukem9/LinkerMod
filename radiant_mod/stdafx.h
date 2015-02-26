#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <intrin.h>
#include <shellapi.h>

#include "minidx9/Include/d3dx9.h"
#pragma comment(lib, "minidx9/Lib/x86/d3dx9.lib")

#pragma comment(lib, "detours/Detours.lib")
#include "detours/Typedef.h"
#include "detours/Detours.h"

#define SRCLINE(x)
#define CHECK_SIZE(Type, Size) static_assert(sizeof(Type) == Size, "Invalid type size!");

#include "assertdef.h"

#include "CWinApp.h"

#include "T5.h"
#include "q_parse.h"
#include "com_files.h"

#include "xmodel_load_obj.h"

#include "r_init.h"
#include "r_utils.h"
#include "r_image_load_common.h"
#include "r_image_load_obj.h"
#include "r_image.h"
#include "r_material_load_obj.h"
#include "r_material.h"