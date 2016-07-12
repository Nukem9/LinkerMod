#pragma once
#include <windows.h>

#define EXCEPTION_STR_MAXLEN 8192
#define EXCEPTION_STACKTRACE_MAXFRAMECOUNT 32

int __cdecl PrivateUnhandledExceptionFilter(_EXCEPTION_POINTERS *ExceptionInfo);
