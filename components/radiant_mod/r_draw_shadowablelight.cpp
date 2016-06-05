#include "r_draw_shadowablelight.h"

R_SetLightProperties_t R_SetLightProperties_o = (R_SetLightProperties_t)0x0056E4A0;

int __cdecl R_SetLightProperties(float* source, void *light, int def, int hasShadowMap, float spotShadowFade)
{
	int result = R_SetLightProperties_o(source, light, def, hasShadowMap, spotShadowFade);

	//
	// Compensate for BO1's change in light radius calculations
	// forces 1.0 / (radius * radius)
	//
	source[579] *= source[579];

	return result;
}