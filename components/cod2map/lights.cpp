#include "lights.h"
#include <Windows.h>
#include "kvs.h"

PrimaryLightData* g_tmpPrimaryLightData = nullptr;
int g_primaryLightCount = 0;

void* rtn_PrimaryLightHandler = (void*)0x0043D64E;
void __declspec(naked) mfh_PrimaryLightHandler(void)
{
	_asm
	{
		pushad
		push eax
		push ebx
		call PrimaryLightHandler
		add esp, 8
		popad

		jmp rtn_PrimaryLightHandler
	}
}

void LoadPrimaryLightData(void* ent, PrimaryLightData* dest)
{
	dest->near_edge = Ent_KV_Float(ent, "near_edge", 0.0f);
	dest->far_edge = Ent_KV_Float(ent, "far_edge", 0.0f);
	dest->cut_on = Ent_KV_Float(ent, "cut_on", 0.0f);
	dest->roundness = Ent_KV_Float(ent, "roundness", 1.0f);

	if (*Ent_GetKV_String(ent, "superellipse") != '\0')
	{
		dest->superellipse = Ent_KV_Vec4(ent, "superellipse", vec4(0.0f, 1.0f, 0.0f, 1.0f));
	}
	else //catch "superellipsis" too
	{
		dest->superellipse = Ent_KV_Vec4(ent, "superellipsis", vec4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	dest->attenuation = Ent_KV_Vec3(ent, "attenuation", vec3(1.0f, 0.0f, 0.0f));
}

void __cdecl PrimaryLightHandler(void* ent, int diskPrimaryLightCount)
{
	if (!g_tmpPrimaryLightData)
	{
		int* p_lightCount = (int*)0x11870C6C;
		*p_lightCount += 2; //Ensure count includes the 2 default lights

		g_tmpPrimaryLightData = new PrimaryLightData[*p_lightCount];
		memset(g_tmpPrimaryLightData, 0, sizeof(PrimaryLightData) * *p_lightCount);
	}

	LoadPrimaryLightData(ent, &g_tmpPrimaryLightData[diskPrimaryLightCount]);
	g_primaryLightCount++;
}

void Light_FixPrimaryLightInfo(Lump* lump)
{
	if (!g_tmpPrimaryLightData)
	{
		Con_Printf("Custom primary light data was not generated (using default values) ...\n");
		return;
	}

	DiskPrimaryLight_BO* lights = (DiskPrimaryLight_BO*)lump->content;
	for (int i = 2; i < g_primaryLightCount + 2; i++)
	{
		lights[i].nearEdge = g_tmpPrimaryLightData[i].near_edge;
		lights[i].farEdge = g_tmpPrimaryLightData[i].far_edge;
		lights[i].cutOn = g_tmpPrimaryLightData[i].cut_on;
		lights[i].roundness = g_tmpPrimaryLightData[i].roundness;
		
		lights[i].attenuation[0] = g_tmpPrimaryLightData[i].attenuation.x;
		lights[i].attenuation[1] = g_tmpPrimaryLightData[i].attenuation.y;
		lights[i].attenuation[2] = g_tmpPrimaryLightData[i].attenuation.z;

		lights[i].aAbB[0] = g_tmpPrimaryLightData[i].superellipse.x;
		lights[i].aAbB[1] = g_tmpPrimaryLightData[i].superellipse.y;
		lights[i].aAbB[2] = g_tmpPrimaryLightData[i].superellipse.z;
		lights[i].aAbB[3] = g_tmpPrimaryLightData[i].superellipse.w;
	}
}