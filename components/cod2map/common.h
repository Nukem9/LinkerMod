#pragma once

typedef int (__cdecl* Com_Printf_t)(char *fmt, ...);
static Com_Printf_t Com_Printf = (Com_Printf_t)0x00418F60;
