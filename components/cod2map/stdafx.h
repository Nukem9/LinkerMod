#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <limits>
#include <math.h>

//
// Shared files
//
#include "../shared/utility.h"

#include "PageGuard.h"

#include "../D3DBSP_Lib/D3DBSP_Lib/D3DBSP_Lib.h"
#include "../D3DBSP_Lib/D3DBSP_Lib/Lightmaps.h"
#include "../D3DBSP_Lib/D3DBSP_Lib/LightGridColors.h"
#if _DEBUG
#pragma comment(lib, "../../build/Debug/D3DBSP_Lib.lib")
#else
#pragma comment(lib, "../../build/Release/D3DBSP_Lib.lib")
#endif


#include "vec.h"
#include "kvs.h"
#include "lights.h"
#include "libqhull_geom.h"
#include "com_files.h"