#include "stdafx.h"

//
// TODO: Allow for default values
//
void __cdecl Ent_BuildLightInfo_WithAdditionalKVs(void* ent)
{
	g_lightInfo.kvs.roundness = Ent_KV_GetFloat(ent, "roundness");

	g_lightInfo.kvs.near_edge = Ent_KV_GetFloat(ent, "near_edge");
	g_lightInfo.kvs.far_edge = Ent_KV_GetFloat(ent, "far_edge");

	g_lightInfo.kvs.cut_on = Ent_KV_GetFloat(ent, "cut_on");
	g_lightInfo.kvs.falloffdistance = Ent_KV_GetFloat(ent, "falloffdistance");

	Ent_KV_GetVec3(ent, "angle", &g_lightInfo.kvs.angle);
	Ent_KV_GetVec3(ent, "attenuation", &g_lightInfo.kvs.attenuation);

	Ent_KV_GetVec4(ent, "superellipse", &g_lightInfo.kvs.superellipse);
}

void* rtn_Ent_BuildLightInfo = (void*)0x0040836C;
void __declspec(naked) mfh_Ent_BuildLightInfo()
{
	_asm
	{
		pushad
		push eax
		call Ent_BuildLightInfo_WithAdditionalKVs
		add esp, 4
		popad;
		
		call Ent_KV_GetFloat_o;
		jmp rtn_Ent_BuildLightInfo;

	}
}

void Ent_GenerateDefaultSpotLightKVs(void* ent)
{
	Ent_KV_SetString_IfMissing(ent, "exponent", "1");
	Ent_KV_SetString_IfMissing(ent, "fov_inner", "60");
	Ent_KV_SetString_IfMissing(ent, "fov_outer", "90");

	Ent_KV_SetString_IfMissing(ent, "roundness", "1");

	Ent_KV_SetString_IfMissing(ent, "cut_on", "0");
	Ent_KV_SetString_IfMissing(ent, "near_edge", "0");
	Ent_KV_SetString_IfMissing(ent, "far_edge", "0");

	Ent_KV_SetString_IfMissing(ent, "falloffdistance", "100");

	Ent_KV_SetString_IfMissing(ent, "angle", "0 0 0");
	Ent_KV_SetString_IfMissing(ent, "attenuation", "1 0 0");
	Ent_KV_SetString_IfMissing(ent, "superellipse", "0 1 0 1");
}

void* rtn_Ent_Connect = (void*)0x004A81AB;
void __declspec(naked) mfh_Ent_Connect() //004A814E
{
	_asm
	{
		pushad
		push ebx //ent
		call Ent_GenerateDefaultSpotLightKVs
		add esp, 4
		popad
		jmp rtn_Ent_Connect
	}
}