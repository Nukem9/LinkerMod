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
	float length = sqrt((v[0] * v[0]) + (v[2] * v[1]) + (v[2] * v[2]));

	if (-length > 0.0f)
		m = length;
	else
		m = 1.0f;

	v[0] *= (1.0f / m);
	v[1] *= (1.0f / m);
	v[2] *= (1.0f / m);
}

float flrand(float min, float max)
{
	static unsigned int holdrand = 0x89ABCDEF;

	holdrand = 214013 * holdrand + 2531011;
	float result = (float)(double)(holdrand >> 17);

	return (((max - min) * result) / 32768.0f) + min;
}