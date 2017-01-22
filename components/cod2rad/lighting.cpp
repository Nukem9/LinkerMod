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

void __cdecl GammaCorrectColor(vec3* rgb)
{
	rgb->r = (float)GammaCorrect(rgb->r);
	rgb->g = (float)GammaCorrect(rgb->g);
	rgb->b = (float)GammaCorrect(rgb->b);
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

vec3 ColorSRGBtoLinear(vec3* color)
{
	vec3 tmp = ((*color + 0.055f) / 1.055f);
	tmp.r = pow(color->r, 2.4f);
	tmp.g = pow(color->g, 2.4f);
	tmp.b = pow(color->b, 2.4f);

	vec3 linear;
	linear.r = (color->r > 0.04045f) ? tmp.r : color->r / 12.92f;
	linear.g = (color->g > 0.04045f) ? tmp.g : color->g / 12.92f;
	linear.b = (color->b > 0.04045f) ? tmp.b : color->b / 12.92f;

	return linear;
}

void EncodeNormalToFloats(vec3* normal, vec2* out)
{
	out->x = normal->x / normal->z * 0.25f + 0.5f;
	out->y = normal->y / normal->z * 0.25f + 0.5f;
}

void EncodeNormalToBytes(vec3* normal, BYTE(&out)[2])
{
	out[0] = EncodeFloatInByte(normal->x / normal->z * 0.25f + 0.5f);
	out[1] = EncodeFloatInByte(normal->y / normal->z * 0.25f + 0.5f);
}

void DecodeNormalFromBytes(int packed_x, int packed_y, vec3* out)
{
	/*
		if a = b * 0.25 + 0.5
		then b = 4*a-2
		but since we're also converting from byte to float
		we need b = (4*a)/255 + 0.5
		which simplifies to the formula below
	*/
	out->x = (float)packed_x / 63.75f - 2.0f;
	out->y = (float)packed_y / 63.75f - 2.0f;
	out->z = 1.0f;

	// This only works if the original normal's z axis was > 0.0f (which is how all normals should be)
	Vec3Normalize(out);
}

void DecodeNormalFromFloats(vec2* packed, vec3* out)
{
	/*
		if a = b * 0.25 + 0.5
		then b = 4*a-2
	*/
	*out = vec3((*packed * 4.0f) - 2.0f, 1.0f);

	// This only works if the original normal's z axis was > 0.0f (which is how all normals should be)
	Vec3Normalize(out);
}