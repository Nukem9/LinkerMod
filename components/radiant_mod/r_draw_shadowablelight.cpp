#include "stdafx.h"

GfxLight* g_lightStack[GFX_LIGHT_TYPE_COUNT_WITH_SHADOWMAP_VERSIONS + 1];

R_SetLightProperties_t R_SetLightProperties_o = (R_SetLightProperties_t)0x0056E4A0;

int __cdecl R_SetLightProperties(float* source, void *light, int def, int hasShadowMap, float spotShadowFade)
{
	GfxLight* _light = (GfxLight*)light;

	_ASSERT(_light->type < GFX_LIGHT_TYPE_COUNT_WITH_SHADOWMAP_VERSIONS);
	g_lightStack[GFX_LIGHT_TYPE_COUNT_WITH_SHADOWMAP_VERSIONS] = _light;
	g_lightStack[_light->type] = _light;

	int result = R_SetLightProperties_o(source, light, def, hasShadowMap, spotShadowFade);

	//
	// Compensate for BO1's change in light radius calculations
	// forces 1.0 / (radius * radius)
	//
	source[579] *= source[579];

	return result;
}

GfxLight* R_GetCurrentLight(void)
{
	return g_lightStack[GFX_LIGHT_TYPE_COUNT_WITH_SHADOWMAP_VERSIONS];
}

GfxLight* R_GetLastDirLight(void)
{
	return g_lightStack[GFX_LIGHT_TYPE_DIR];
}

GfxLight* R_GetLastSpotLight(void)
{
	return g_lightStack[GFX_LIGHT_TYPE_SPOT];
}

GfxLight* R_GetLastOmniLight(void)
{
	return g_lightStack[GFX_LIGHT_TYPE_OMNI];
}
