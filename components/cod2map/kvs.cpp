#include "kvs.h"
#include <iostream>

int Ent_GetKV_Vec4(void* ent, const char* key, vec4* dest)
{
	const char* v = Ent_GetKV_String(ent, key);
	dest->x = 0.0f;
	dest->y = 0.0f;
	dest->z = 0.0f;
	dest->w = 0.0f;

	int result = sscanf(v, "%f %f %f %f", &dest->x, &dest->y, &dest->z, &dest->w);
	if (result != 4)
	{
		result = printf("WARNING: entity: key '%s' with value '%s', when it should be a vector; treating as '%g %g %g %g'\n",
			key, v, dest->x, dest->y, dest->z, dest->w);
	}

	return result;
}

float Ent_KV_Float(void* ent, const char* key, float defaultValue)
{
	if (*Ent_GetKV_String(ent, key) == '\0')
	{
		return defaultValue;
	}

	return Ent_GetKV_Float(ent, key);
}

vec3 Ent_KV_Vec3(void* ent, const char* key, vec3 defaultValue)
{
	if (*Ent_GetKV_String(ent, key) == '\0')
	{
		return defaultValue;
	}

	vec3 dest = { 0.0f, 0.0f, 0.0f };
	if (Ent_GetKV_Vec3(ent, key, &dest) != 3)
	{
		return defaultValue;
	}

	return dest;
}

vec4 Ent_KV_Vec4(void* ent, const char* key, vec4 defaultValue)
{
	if (*Ent_GetKV_String(ent, key) == '\0')
	{
		return defaultValue;
	}

	vec4 dest = { 0.0f, 0.0f, 0.0f, 0.0f};
	if (Ent_GetKV_Vec4(ent, key, &dest) != 4)
	{
		return defaultValue;
	}

	return dest;
}