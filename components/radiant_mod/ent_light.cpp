#include "stdafx.h"

void* KV_GetFloat_o = (void*)0x0049CF50;

float KV_GetFloat(void* ent, const char* str)
{
	void* _ent = ent;
	const char* _str = str;

	float result;

	_asm
	{
		mov eax, _ent
		mov edi, _str
		call KV_GetFloat_o
		fstp result
	}

	return result;
}

void __cdecl R_BuildLightFromEntity_WithAdditionalKVs(void* ent)
{
	g_lightInfo.roundness = KV_GetFloat(ent, "roundness");
}

void* rtn_R_BuildLightFromEntity = (void*)0x0040836C;
void __declspec(naked) mfh_R_BuildLightFromEntity()
{
	_asm
	{
		pushad
		push eax
		call R_BuildLightFromEntity_WithAdditionalKVs
		add esp, 4
		popad;
		
		call KV_GetFloat_o;
		jmp rtn_R_BuildLightFromEntity;

	}
}