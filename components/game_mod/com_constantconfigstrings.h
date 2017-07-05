#pragma once

#include "vanilla.h"

VANILLA_FUNC(CCS_GetChecksum, int(__cdecl*)(void), 0x004ACAB0);
VANILLA_FUNC(CCS_GetConfigStringNum, int(__cdecl*)(const int rowNum), 0x00471F80);
