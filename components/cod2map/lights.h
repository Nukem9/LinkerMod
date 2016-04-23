#pragma once
#include "vec.h"

#include "../D3DBSP_Lib/D3DBSP_Lib/D3DBSP_Lib.h"
#include "../D3DBSP_Lib/D3DBSP_Lib/PrimaryLights.h"
#if _DEBUG
#pragma comment(lib, "../build/Debug/D3DBSP_Lib.lib")
#else
#pragma comment(lib, "../build/Release/D3DBSP_Lib.lib")
#endif


struct PrimaryLightData
{
	float near_edge;
	float far_edge;
	float cut_on;
	vec4 superellipse;
	float roundness;
	vec3 attenuation;
};

extern PrimaryLightData* g_tmpPrimaryLightData;

void mfh_PrimaryLightHandler(void);
void __cdecl PrimaryLightHandler(void* ent, int diskPrimaryLightCount);

void Light_FixPrimaryLightInfo(Lump* lump);
