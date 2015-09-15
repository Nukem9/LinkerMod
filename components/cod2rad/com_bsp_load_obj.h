#pragma once

#include <wtypes.h>

size_t __cdecl Com_LoadBsp_EnforceVersion(BYTE** data);
int __cdecl Com_SaveBsp_EnforceVersion(FILE* h);

void mfh_Com_SaveBsp();