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

//
// Clamp src to 0.0 - 'max' range, returns true if the value needed to be clamped
//
bool ClampColor(vec3 *dst, vec3 *src, float max)
{
	bool clamped = false;

	if (src->r >= 0.0f)
	{
		if (src->r <= max)
		{
			dst->r = src->r;
		}
		else
		{
			dst->r = max;
			clamped = true;
		}
	}
	else
	{
		dst->r = 0.0f;
		clamped = true;
	}

	if (src->g >= 0.0f)
	{
		if (src->g <= max)
		{
			dst->g = src->g;
		}
		else
		{
			dst->g = max;
			clamped = true;
		}
	}
	else
	{
		dst->g = 0.0f;
		clamped = true;
	}

	if (src->b >= 0.0f)
	{
		if (src->b <= max)
		{
			dst->b = src->b;
		}
		else
		{
			dst->b = max;
			clamped = true;
		}
	}
	else
	{
		dst->b = 0.0f;
		clamped = true;
	}

	return clamped;
}
