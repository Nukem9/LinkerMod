#pragma once

#pragma comment(lib, "detours.lib")
#include "../shared/detours/Detours.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <intrin.h>
#include <shellapi.h>

#define RADIANT_MOD 1
#define BO1_SEMI_NATIVE_BUILD 1

#define RADIANT_USE_AFX_OVERRIDES 0
#define RADIANT_USE_SPLASH 1

#define RADIANT_DISABLE_SPAM_MSG_SCAN 1
#define RADIANT_DISABLE_SPAM_MSG_IMAGE 1
#define RADIANT_DISABLE_SPAM_MSG_MATERIAL 1

#define RADIANT_MOD_VERBOSE 0
#if RADIANT_MOD_VERBOSE
#define _VERBOSE(X) X;
#else
#define _VERBOSE(X)
#endif

#define APP_SPLASH_PATH TEXT("../cod.bmp")
#define APP_SPLASH_TITLE TEXT("CoDBORadiant")

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

#if WAW_PIMP
#define PATH_PIMP_FALLBACK "waw_pimp"
#define TECH_TYPE_COUNT 59
#else
#define PATH_PIMP_FALLBACK "pimp"
#define TECH_TYPE_COUNT 59 //130 causes radiant to crash at the moment
#endif

#define SRCLINE(x)
#define CHECK_SIZE(Type, Size) static_assert(sizeof(Type) == Size, "Invalid type size!");

#include "d3d9ex.h"
#include "CWinApp.h"
#include "win_main.h"
#include "win_splash.h"

#include "T5.h"
#include "com_files.h"
#include "com_memory.h"
#include "com_math.h"

#include "q_parse.h"
#include "q_project.h"
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

#include "r_shade.h"
#include "r_draw_shadowablelight.h"

#include "kvs.h"
#include "ent_light.h"

#include "afx.h"
#include "afx_colors.h"
#include "afx_resource.h"
