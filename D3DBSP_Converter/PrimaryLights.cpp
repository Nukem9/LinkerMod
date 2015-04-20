#include "PrimaryLights.h"
#include "Lump.h"

int ConvertLump_WAWtoBO_PrimaryLights(Lump* wawLump, Lump* boLump)
{
	DWORD PrimaryLightCount = wawLump->size / sizeof(DiskPrimaryLight_WAW);

	DiskPrimaryLight_WAW* wawLights = (DiskPrimaryLight_WAW*)wawLump->content;
	DiskPrimaryLight_BO* boLights = (DiskPrimaryLight_BO*)boLump->AllocateMemory(sizeof(DiskPrimaryLight_BO) * PrimaryLightCount);

	for(DWORD lightIndex = 0; lightIndex < PrimaryLightCount; lightIndex++)
	{
		memset(&boLights[lightIndex],0,sizeof(DiskPrimaryLight_BO));

		boLights[lightIndex].type = wawLights[lightIndex].type;
		boLights[lightIndex].canUseShadowMap = wawLights[lightIndex].canUseShadowMap;
		boLights[lightIndex].cullDist = wawLights[lightIndex].cullDist;
		boLights[lightIndex].priority = wawLights[lightIndex].priority;
		boLights[lightIndex].color[0] = wawLights[lightIndex].color[0];
		boLights[lightIndex].color[1] = wawLights[lightIndex].color[1];
		boLights[lightIndex].color[2] = wawLights[lightIndex].color[2];
		boLights[lightIndex].dir[0] = wawLights[lightIndex].dir[0];
		boLights[lightIndex].dir[1] = wawLights[lightIndex].dir[1];
		boLights[lightIndex].dir[2] = wawLights[lightIndex].dir[2];
		boLights[lightIndex].origin[0] = wawLights[lightIndex].origin[0];
		boLights[lightIndex].origin[1] = wawLights[lightIndex].origin[1];
		boLights[lightIndex].origin[2] = wawLights[lightIndex].origin[2];
		boLights[lightIndex].radius = wawLights[lightIndex].radius;
		boLights[lightIndex].cosHalfFovOuter = wawLights[lightIndex].cosHalfFovOuter;
		boLights[lightIndex].cosHalfFovInner = wawLights[lightIndex].cosHalfFovInner;
		boLights[lightIndex].rotationLimit = wawLights[lightIndex].rotationLimit;
		boLights[lightIndex].translationLimit = wawLights[lightIndex].translationLimit;
		
		if(lightIndex > 1)
		{
			boLights[lightIndex].cutOn = 0;
	
			boLights[lightIndex].nearEdge = 0;
			boLights[lightIndex].farEdge = 0;

			boLights[lightIndex].specularcolor[0] = boLights[lightIndex].color[0];
			boLights[lightIndex].specularcolor[1] = boLights[lightIndex].color[1];
			boLights[lightIndex].specularcolor[2] = boLights[lightIndex].color[2];
			boLights[lightIndex].bouncecolor[0] = -1.0f;
			boLights[lightIndex].bouncecolor[1] = -1.0f;
			boLights[lightIndex].bouncecolor[2] = -1.0f;
			boLights[lightIndex].angle[0] = 0.0f;			//matches "angle" in radiant (not "angles" which is plural)
			boLights[lightIndex].angle[1] = 0.0f;
			boLights[lightIndex].angle[2] = 0.0f;
			boLights[lightIndex].attenuation[0] = 1.0f;		//matches attenuation in radiant
			boLights[lightIndex].attenuation[1] = 0.0f;
			boLights[lightIndex].attenuation[2] = 0.0f;
			boLights[lightIndex].aAbB[0] = 0.75f;			//matches superellipse in radiant
			boLights[lightIndex].aAbB[1] = 1.0f;
			boLights[lightIndex].aAbB[2] = 0.75f;
			boLights[lightIndex].aAbB[3] = 1.0f;
			boLights[lightIndex].cookieControl0[0] = 0.0f;
			boLights[lightIndex].cookieControl0[1] = 0.0f;
			boLights[lightIndex].cookieControl0[2] = 1.0f;
			boLights[lightIndex].cookieControl0[3] = 1.0f;
			boLights[lightIndex].cookieControl1[0] = 0.0f;
			boLights[lightIndex].cookieControl1[1] = 0.0f;
			boLights[lightIndex].cookieControl1[2] = 0.0f;
			boLights[lightIndex].cookieControl1[3] = 0.0f;
			boLights[lightIndex].cookieControl2[0] = 0.0f;
			boLights[lightIndex].cookieControl2[1] = 0.0f;
			boLights[lightIndex].cookieControl2[2] = 0.0f;
			boLights[lightIndex].cookieControl2[3] = 0.0f;
		}

		memcpy(boLights[lightIndex].defName,wawLights[lightIndex].defName,64);
	}

	return 0;
}

int ConvertLump_BOtoWAW_PrimaryLights(Lump* boLump, Lump* wawLump)
{
	DWORD PrimaryLightCount = boLump->size / sizeof(DiskPrimaryLight_BO);

	DiskPrimaryLight_BO* BOLights = (DiskPrimaryLight_BO*)boLump->content;
	DiskPrimaryLight_WAW* WAWLights = (DiskPrimaryLight_WAW*)wawLump->AllocateMemory(sizeof(DiskPrimaryLight_WAW) * PrimaryLightCount);

	for(DWORD lightIndex = 0; lightIndex < PrimaryLightCount; lightIndex++)
	{
		memset(&WAWLights[lightIndex],0,sizeof(DiskPrimaryLight_WAW));

		WAWLights[lightIndex].type = BOLights[lightIndex].type;
		WAWLights[lightIndex].canUseShadowMap = BOLights[lightIndex].canUseShadowMap;
		WAWLights[lightIndex].cullDist = BOLights[lightIndex].cullDist;
		WAWLights[lightIndex].priority = BOLights[lightIndex].priority;
		WAWLights[lightIndex].color[0] = BOLights[lightIndex].color[0];
		WAWLights[lightIndex].color[1] = BOLights[lightIndex].color[1];
		WAWLights[lightIndex].color[2] = BOLights[lightIndex].color[2];
		WAWLights[lightIndex].dir[0] = BOLights[lightIndex].dir[0];
		WAWLights[lightIndex].dir[1] = BOLights[lightIndex].dir[1];
		WAWLights[lightIndex].dir[2] = BOLights[lightIndex].dir[2];
		WAWLights[lightIndex].origin[0] = BOLights[lightIndex].origin[0];
		WAWLights[lightIndex].origin[1] = BOLights[lightIndex].origin[1];
		WAWLights[lightIndex].origin[2] = BOLights[lightIndex].origin[2];
		WAWLights[lightIndex].radius = BOLights[lightIndex].radius;
		WAWLights[lightIndex].cosHalfFovOuter = BOLights[lightIndex].cosHalfFovOuter;
		WAWLights[lightIndex].cosHalfFovInner = BOLights[lightIndex].cosHalfFovInner;
		WAWLights[lightIndex].rotationLimit = BOLights[lightIndex].rotationLimit;
		WAWLights[lightIndex].translationLimit = BOLights[lightIndex].translationLimit;

		memcpy(WAWLights[lightIndex].defName,BOLights[lightIndex].defName,64);
	}

	return 0;
}