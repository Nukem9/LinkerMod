#pragma once

#include <wtypes.h>

template <typename T>
union tvec2
{
	struct
	{
		T r;
		T g;
	};

	struct
	{
		T x;
		T y;
	};
};

template <typename T>
union tvec3
{
	struct
	{
		T r;
		T g;
		T b;
	};

	struct
	{
		T x;
		T y;
		T z;
	};
};

template <typename T>
union tvec4
{
	struct
	{
		T r;
		T g;
		T b;
		T a;
	};

	struct
	{
		T x;
		T y;
		T z;
		T w;
	};
};

typedef tvec2<float> vec2;
typedef tvec3<float> vec3;
typedef tvec4<float> vec4;

void Vec3Normalize(vec3* v);
void Vec3Normalize(float* v);

float Vec3Dot(vec3* a, vec3* b);
float Vec3Dot(float* a, float* b);
