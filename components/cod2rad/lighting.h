#pragma once

static int& g_basisDirectionsCount = *(int*)0x16E99F70;
static vec3*& g_basisDirections = *(vec3**)0x16E99F74;

static float& g_Gamma = *(float*)0x153C907C;

double __cdecl GammaCorrect(float color);
void __cdecl GammaCorrectColor(float *rgb);

bool ClampColor(vec3 *dst, vec3 *src);
