#pragma once
#include "vec.h"

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
