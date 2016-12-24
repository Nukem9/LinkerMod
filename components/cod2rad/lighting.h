#pragma once

static float& g_Gamma = *(float*)0x153C907C;

double __cdecl GammaCorrect(float color);
void __cdecl GammaCorrectColor(float *rgb);

