#include "stdafx.h"

void Vec3Normalize(vec3* v)
{
	float m = (float)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
	v->x /= m;
	v->y /= m;
	v->z /= m;
}

void Vec3Normalize(float* v)
{
	Vec3Normalize((vec3*)v);
}

float Vec3Dot(vec3* a, vec3* b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

float Vec3Dot(float* a, float* b)
{
	return Vec3Dot((vec3*)a, (vec3*)b);
}
