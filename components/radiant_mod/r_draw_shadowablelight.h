#pragma once

typedef int(__cdecl* R_SetLightProperties_t)(float* source, void *light, int def, int hasShadowMap, float spotShadowFade);
extern R_SetLightProperties_t R_SetLightProperties_o;

int __cdecl R_SetLightProperties(float* source, void *light, int def, int hasShadowMap, float spotShadowFade);
