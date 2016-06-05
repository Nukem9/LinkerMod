#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <intrin.h>
#include <shellapi.h>

#ifndef RADIANT_MOD
#define RADIANT_MOD 1
#endif

#ifndef BO1_SEMI_NATIVE_BUILD
#define BO1_SEMI_NATIVE_BUILD 1
#endif

//
// Shared files
//
#include "../shared/utility.h"

#include "../shared/minidx9/Include/d3dx9.h"
#pragma comment(lib, "../shared/minidx9/Lib/x86/d3dx9.lib")
//
// 
//

#if WAW_PIMP
#define PATH_PIMP_FALLBACK "waw_pimp"
#define TECH_TYPE_COUNT 59
#else
#define PATH_PIMP_FALLBACK "pimp"
#define TECH_TYPE_COUNT 59 //130 causes radiant to crash at the moment
#endif

#define SRCLINE(x)
#define CHECK_SIZE(Type, Size) static_assert(sizeof(Type) == Size, "Invalid type size!");

#include "CWinApp.h"
#include "win_main.h"

#include "T5.h"
#include "com_files.h"
#include "com_memory.h"

#include "q_parse.h"
#include "xmodel_load_obj.h"
#include "r_xsurface_load_obj.h"

#include "r_init.h"
#include "r_utils.h"
#include "r_image_load_common.h"
#include "r_image_load_obj.h"
#include "r_image.h"
#include "r_material_load_obj.h"
#include "r_material_load_obj_waw.h"
#include "r_material.h"

#include "r_draw_shadowablelight.h"
