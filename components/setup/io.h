#pragma once

#include <iostream>
#include <stdarg.h>

//int printf(const char* fmt, ...);

//
// printf for verbose mode
// prints only when verbose mode is active
//
int printf_v(const char* fmt, ...);

//
// printf for verbose mode
// prints only when verbose mode is NOT active
//
int printf_nv(const char* fmt, ...);
