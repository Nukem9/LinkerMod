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

#define isatty		_isatty
#define fileno		_fileno
#define strdup		_strdup
#define snprintf	_snprintf

#define W32_WARNING_DISABLE(NUM)	__pragma(warning(disable:NUM))
#define W32_WARNING_ENABLE(NUM)		__pragma(warning(default:NUM))

W32_WARNING_DISABLE(4996) // Disable POSIX Name Deprecation

#ifndef _SSIZE_T_DEFINED
#ifdef _WIN64
typedef signed __int64    ssize_t;
#else  /* _WIN64 */
typedef _W64 signed int   ssize_t;
#endif  /* _WIN64 */
#define _SSIZE_T_DEFINED
#endif 

ssize_t getline(char **lineptr, size_t *n, FILE *stream);

#else //LINUX

#include <unistd.h>	// required for isatty (LINUX)
#define stricmp strcasecmp

#endif
