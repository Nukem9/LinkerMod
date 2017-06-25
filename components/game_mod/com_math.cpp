#include "stdafx.h"

float Vec3DistanceSq(const float *p1, const float *p2)
{
	float v[3];
	v[0] = p2[0] - p1[0];
	v[1] = p2[1] - p1[1];
	v[2] = p2[2] - p1[2];

	return (v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]);
}

void Vec3Normalize(float *v)
{
	float m;
	float length = sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));

	if (length <= 0.0f)
		m = 1.0f;
	else
		m = 1.0f / length;

	v[0] *= m;
	v[1] *= m;
	v[2] *= m;
}

float flrand(float min, float max)
{
	static unsigned int holdrand = 0x89ABCDEF;

	holdrand = 214013 * holdrand + 2531011;
	float result = (float)(double)(holdrand >> 17);

	return (((max - min) * result) / 32768.0f) + min;
}

void __cdecl AxisCopy(const float(*in)[3], float(*out)[3])
{
	(*out)[0] = (*in)[0];
	(*out)[1] = (*in)[1];
	(*out)[2] = (*in)[2];
	(*out)[3] = (*in)[3];
	(*out)[4] = (*in)[4];
	(*out)[5] = (*in)[5];
	(*out)[6] = (*in)[6];
	(*out)[7] = (*in)[7];
	(*out)[8] = (*in)[8];
}
