#pragma once

struct DebugGlobals
{
	char data[0x6C];
};

static float* const colorWhite = (float* const)0x00A5E8C4;
static float* const colorRed = (float* const)0x00A5E794;

void __cdecl R_AddDebugString(DebugGlobals *debugGlobalsEntry, const float *origin, const float *color, float scale, const char *string);
