#pragma once

//
// Include these headers regardless of platform to preserve
// consistent header functionality
//
#include <stdio.h>	// required for _fileno & fopen_s (WIN32)
#include <string.h>	// require for _strdup (WIN32)

#if _WIN32

// WIN32 Specific ( required for _isatty )
#include <io.h>
#if _CRTDBG_MAP_ALLOC 
#include <crtdbg.h>
#endif

#define isatty		_isatty
#define fileno		_fileno
#ifndef _INC_CRTDBG
#define strdup		_strdup
#endif
#define snprintf	_snprintf

#define W32_WARNING_DISABLE(NUM)	__pragma(warning(disable:NUM))
#define W32_WARNING_ENABLE(NUM)		__pragma(warning(default:NUM))

W32_WARNING_DISABLE(4996) // Disable POSIX Name Deprecation

#include <assert.h>
#include "common\io.h"

#else //LINUX

#include <unistd.h>	// required for isatty (LINUX)
#define stricmp strcasecmp

#endif
