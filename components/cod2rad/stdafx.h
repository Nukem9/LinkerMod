#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#define CHECK_SIZE(Type, Size) static_assert(sizeof(Type) == Size, "Invalid type size!");

//
// Shared files
//
#include "../shared/utility.h"

#include "PageGuard.h"
#include "console.h"
#include "threading.h"

#include "arguments.h"
#include "vector.h"
#include "hdr.h"

#include "r_lightmaps.h"
#include "r_lightgrid.h"
#include "r_xmodel_load_obj.h"
#include "r_xsurface_load_obj.h"

#include "common.h"
#include "com_memory.h"
#include "com_files.h"

#include "com_bsp_load_obj.h"
#include "r_xmodel_load_obj.h"
#include "r_xsurface_load_obj.h"
#include "r_image_load_obj.h"

#include "../D3DBSP_Lib/D3DBSP_Lib/D3DBSP_Lib.h"
#include "../D3DBSP_Lib/D3DBSP_Lib/Lightmaps.h"
#include "../D3DBSP_Lib/D3DBSP_Lib/LightGridColors.h"
#if _DEBUG
#pragma comment(lib, "../../build/Debug/D3DBSP_Lib.lib")
#else
#pragma comment(lib, "../../build/Release/D3DBSP_Lib.lib")
#endif
