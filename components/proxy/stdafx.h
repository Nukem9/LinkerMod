#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdlib.h>
#include <vector>

BOOL WINAPI Proxy_Init(HINSTANCE hInst, DWORD reason);
BOOL WINAPI Proxy_Free(HINSTANCE hInst, DWORD reason);
