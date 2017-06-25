#include "stdafx.h"

// /client/cl_debugdata.cpp:223
void CL_AddDebugLine(const float *start, const float *end, const float *color, int depthTest, int duration)
{
	((void(__cdecl *)(const float *, const float *, const float *, int, int))0x004C9AF0)(start, end, color, depthTest, duration);
}

// This is the wrong file
void CG_DebugLine(const float *start, const float *end, const float *color, int depthTest, int duration)
{
	CL_AddDebugLine(start, end, color, depthTest, duration);
}