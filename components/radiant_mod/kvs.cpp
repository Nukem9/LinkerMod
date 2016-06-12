#include "stdafx.h"

//
// Returns a pointer to the string value for the given key
// if no entry under that key is found, a pointer to a zero-length null terminated
// string is returned
//
void* Ent_KV_GetString_o = (void*)0x0049C3A0;
const char* Ent_KV_GetString(void* ent, const char* key)
{
	const char* result;

	_asm
	{
		pushad
		mov edi, key //key
		mov eax, ent //ent
		call Ent_KV_GetString_o
		mov result, eax
		popad
	}

	return result;
}

void* Ent_KV_GetFloat_o = (void*)0x0049CF50;
float Ent_KV_GetFloat(void* ent, const char* str)
{
	void* _ent = ent;
	const char* _str = str;

	float result;

	_asm
	{
		mov eax, _ent
		mov edi, _str
		call Ent_KV_GetFloat_o
		fstp result
	}

	return result;
}


void* Ent_KV_GetVec3_o = (void*)0x0049CFF0;
bool Ent_KV_GetVec3(void* ent, const char* key, vec3* dest)
{
	bool result;

	_asm
	{
		pushad
		push dest //dest
		mov edi, key //key
		mov eax, ent //ent
		call Ent_KV_GetVec3_o
		add esp, 4
		mov result, al
		popad
	}

	return result;
}

int Ent_KV_GetVec4(void* ent, const char* key, vec4* dest)
{
	const char* v = Ent_KV_GetString(ent, key);
	dest->x = 0.0f;
	dest->y = 0.0f;
	dest->z = 0.0f;
	dest->w = 0.0f;

	int result = sscanf(v, "%f %f %f %f", &dest->x, &dest->y, &dest->z, &dest->w);
	/*if (result != 4)
	{
		result = printf("WARNING: entity: key '%s' with value '%s', when it should be a vec4; treating as '%g %g %g %g'\n",
			key, v, dest->x, dest->y, dest->z, dest->w);
	}*/

	return result;
}

Ent_KV_SetString_t Ent_KV_SetString = (Ent_KV_SetString_t)0x0049CE00;

bool Ent_KV_SetString_IfMissing(void* ent, const char* key, char* value)
{
	if (!Ent_KV_HasKey(ent, key))
	{
		Ent_KV_SetString(ent, key, value);
	}
	
	return false;
}

extern void* Ent_KV_HasKey_o = (void*)0x0049D050;
bool Ent_KV_HasKey(void* ent, const char* key)
{
	bool result;

	_asm
	{
		pushad
		mov edi, key //key
		mov eax, ent //ent
		call Ent_KV_HasKey_o
		mov result, al
		popad
	}

	return result;
}
