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

void SpotLightProjectionMatrix(float cosFov, float zNear, float zFar, float* mtx)
{
	memset(mtx, 0, 0x40u);

	if (zNear < 0.001)
		zNear = 0.001f;

	mtx[0] = 1.0f / (sqrt(1.0f - (cosFov * cosFov)) / cosFov);
	mtx[5] = mtx[0];
	mtx[10] = zFar / (zFar - zNear);
	mtx[11] = 1.0f;
	mtx[14] = -mtx[10] * zNear;
}

void SpotLightViewMatrix(const float *direction, float rotation, float* mtx)
{
	rotation = 0.785398f;

	float vec[3];
	vec[0] = -direction[0];
	vec[1] = -direction[1];
	vec[2] = -direction[2];

	float vec3[3];
	PerpendicularVector(vec, vec3);

	float vec2[3];
	Vec3Cross(vec3, vec, vec2);

	float rotationMatrix[16] =
	{	-vec2[0],	vec3[0],	vec[0],		0.0f,
		-vec2[1],	vec3[1],	vec[1],		0.0f,
		-vec2[2],	vec3[2],	vec[2],		0.0f,
		0.0f,		0.0f,		0.0f,		1.0f	};

	float lookAtMatrix[16] =
	{	cos(rotation),	-sin(rotation), 0.0f,	0.0f,
		sin(rotation),	cos(rotation),	0.0f,	0.0f,
		0.0f,			0.0f,			1.0f,	0.0f,
		0.0f,			0.0f,			0.0f,	1.0f	};

	MatrixMultiply44(rotationMatrix, lookAtMatrix, mtx);
}
