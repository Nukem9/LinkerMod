#pragma once

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

#define SRCLINE(x)
#define CHECK_SIZE(Type, Size) static_assert(sizeof(Type) == Size, "Invalid type size!");

#include "T5.h"
#include "com_files.h"
#include "com_memory.h"
#include "com_bsp_load_obj.h"

#include "q_parse.h"
#include "xmodel_load_obj.h"

#include "r_init.h"
#include "r_utils.h"
#include "r_image_load_common.h"
#include "r_image_load_obj.h"
#include "r_image.h"
#include "r_material_load_obj.h"
#include "r_material.h"