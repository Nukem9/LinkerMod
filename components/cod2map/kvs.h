#pragma once

#include "vec.h"

typedef const char* (__cdecl* Ent_GetKV_String_t)(void* ent, const char* key);
static Ent_GetKV_String_t Ent_GetKV_String = (Ent_GetKV_String_t)0x0040BE80;

typedef double (__cdecl* Ent_GetKV_Float_t)(void* ent, const char* key);
static Ent_GetKV_Float_t Ent_GetKV_Float = (Ent_GetKV_Float_t)0x0040BE90;

typedef int (__cdecl* Ent_GetKV_Vec3_t)(void* ent, const char* key, vec3* dest);
static Ent_GetKV_Vec3_t Ent_GetKV_Vec3 = (Ent_GetKV_Vec3_t)0x0040BEE0;

int Ent_GetKV_Vec4(void* ent, const char* key, vec4* dest);

//
// The Following functions are used for assigning default values
// when a given key doesnt actually exist for a given entity
//
float Ent_KV_Float(void* ent, const char* key, float defaultValue);
vec3 Ent_KV_Vec3(void* ent, const char* key, vec3 defaultValue);
vec4 Ent_KV_Vec4(void* ent, const char* key, vec4 defaultValue);
