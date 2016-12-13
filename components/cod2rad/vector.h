#pragma once

#include <wtypes.h>

template <typename T>
struct tvec3
{
	T r;
	T g;
	T b;
};

template <typename T>
struct tvec4
{
	T r;
	T g;
	T b;
	T a;
};

typedef tvec3<float> vec3;
typedef tvec4<float> vec4;
