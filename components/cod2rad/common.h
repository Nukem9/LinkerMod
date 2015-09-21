#pragma once

typedef int(__cdecl* Com_FatalError_t)(char* format, ...); 
static Com_FatalError_t Com_FatalError = (Com_FatalError_t)0x004294B0;
