#pragma once

#include "com_math.h"

extern void* Ent_KV_GetString_o;
const char* Ent_KV_GetString(void* ent, const char* key);

extern void* Ent_KV_GetFloat_o;
float Ent_KV_GetFloat(void* ent, const char* str);

extern void* Ent_KV_GetVec3_o;
bool Ent_KV_GetVec3(void* ent, const char* key, vec3* dest);

int Ent_KV_GetVec4(void* ent, const char* key, vec4* dest);

typedef void(__cdecl* Ent_KV_SetString_t)(void *ent, const char *key, char *value);
extern Ent_KV_SetString_t Ent_KV_SetString;

//
// Sets the key's value only if there is not existing value for that key
// returns true if the value was modified
//
bool Ent_KV_SetString_IfMissing(void* ent, const char* key, char* value);

extern void* Ent_KV_HasKey_o;
bool Ent_KV_HasKey(void* ent, const char* key);
