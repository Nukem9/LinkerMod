#pragma once

typedef int (__cdecl * CM_BoxSightTrace_t)(int oldHitNum, const float *start, const float *end, const float *mins, const float *maxs, unsigned int model, int brushmask);
static CM_BoxSightTrace_t CM_BoxSightTrace = (CM_BoxSightTrace_t)0x4BA600;