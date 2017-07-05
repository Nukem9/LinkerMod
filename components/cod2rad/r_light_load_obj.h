#pragma once

#include "r_imagedecode.h"

struct __declspec(align(4)) GfxLightImage
{
	GfxImage *image;
	char samplerState;
};

struct GfxLightDef
{
	const char *name;
	GfxLightImage attenuation;
	int lmapLookupStart;
};

struct LightGlobals
{
	int defCount;
	GfxLightDef lightDefs[64];
};

static LightGlobals& pointLightGlob = *(LightGlobals*)0x172DE010;

GfxLightDef *__cdecl RegisterLightDef(char *lightDefName);
