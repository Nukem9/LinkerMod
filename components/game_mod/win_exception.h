#pragma once

#define EXCEPTION_STR_MAXLEN 8192
#define EXCEPTION_STACKTRACE_MAXFRAMECOUNT 32

void Patch_ExceptionFilter(void* ExceptionCallback_f, HMODULE hModule);

LONG WINAPI PrivateUnhandledExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo);
const char *ExceptionCodeToString(DWORD ExceptionCode);
