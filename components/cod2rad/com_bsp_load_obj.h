#pragma once

#include "../D3DBSP_Lib/D3DBSP_Lib/D3DBSP_Lib.h"
#include <wtypes.h>

extern Lump preservedPrimaryLights;

size_t __cdecl Com_LoadBsp_EnforceVersion(BYTE** data);
int __cdecl Com_SaveBsp_EnforceVersion(FILE* h);

void mfh_Com_SaveBsp();
