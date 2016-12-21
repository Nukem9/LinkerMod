#include "stdafx.h"

double __cdecl GammaCorrect(float color)
{
	long double _color = color;
	if (_color > 0.0)
	{
		return pow(_color, 1.0 / g_Gamma);
	}

	return 0.0;
}

void __cdecl GammaCorrectColor(float *rgb)
{
	rgb[0] = (float)GammaCorrect(rgb[0]);
	rgb[1] = (float)GammaCorrect(rgb[1]);
	rgb[2] = (float)GammaCorrect(rgb[2]);
}
