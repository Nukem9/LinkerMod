#include "stdafx.h"

GfxLight* g_lightStack[GFX_LIGHT_TYPE_COUNT_WITH_SHADOWMAP_VERSIONS + 1];
GfxLightInfo g_lightInfo;

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

	if (_light->type == GFX_LIGHT_TYPE_SPOT)
	{
		R_BuildSpotLightInfo(source, _light, spotShadowFade);
	}

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

void R_BuildSpotLightInfo(float* source, GfxLight* light, float spotShadowFade)
{
	ASSERT(light->type == GFX_LIGHT_TYPE_SPOT);

	const float roundness = g_lightInfo.kvs.roundness;

	const float cutOn = g_lightInfo.kvs.cut_on; //0.0f;
	const float nearEdge = g_lightInfo.kvs.near_edge; //0.0f;
	const float farEdge = g_lightInfo.kvs.far_edge; //0.0f;

	vec4 aAbB; // = { 0.0f, 1.0f, 0.0f, 1.0f };
	memcpy(&aAbB, &g_lightInfo.kvs.superellipse, sizeof(aAbB));

	vec4 attenuation; // = { 1.0f, 0.0f, 0.0f, 0.0f };
	memcpy(&attenuation, &g_lightInfo.kvs.attenuation, sizeof(attenuation));

	vec4 fallOff;
	fallOff.x = cutOn;
	fallOff.y = light->radius;
	fallOff.z = ((light->radius - cutOn) * nearEdge) + cutOn;
	fallOff.w = ((light->radius - cutOn) * farEdge) + cutOn;

	memcpy(&g_lightInfo.aAbB, &aAbB, sizeof(g_lightInfo.aAbB));
	memcpy(&g_lightInfo.attenuation, &attenuation, sizeof(g_lightInfo.attenuation));

	vec4 angles = { 0.0f, 0.0f, 0.0f, 0.0f };
	memcpy(&angles, light->angles, sizeof(float) * 3);

	R_CalculateCodeConst_LightConeControl1(roundness, &g_lightInfo.coneControl1);
	R_CalculateCodeConst_LightConeControl2(&aAbB, &g_lightInfo.coneControl2);
	R_CalculateCodeConst_LightFallOffA(&aAbB, &fallOff, &g_lightInfo.fallOffA);
	R_CalculateCodeConst_LightFallOffB(&aAbB, &fallOff, &g_lightInfo.fallOffB);

	//
	// Build spotFactors
	//
	g_lightInfo.spotFactors[0] = 0.5f;
	g_lightInfo.spotFactors[1] = 0.0f;
	g_lightInfo.spotFactors[2] = 0.5f;
	g_lightInfo.spotFactors[3] = spotShadowFade;

	float viewMatrix[16];
	SpotLightViewMatrix((float*)&angles, 0.0f, viewMatrix);

	float projMatrix[16];
	SpotLightProjectionMatrix(light->cosHalfFovOuter, fallOff.x, light->radius, projMatrix);

	//
	// Calculate the offset used in the light transformation
	//
	float offset[3];
	Vec3Subtract(light->origin, &source[0x4BF], offset);

	float inverseOffset[3] = { offset[2], offset[1], offset[0] };
	viewMatrix[12] = -(offset[0] * viewMatrix[0] + offset[1] * viewMatrix[4] + offset[2] * viewMatrix[8]);
	viewMatrix[13] = -(inverseOffset[2] * viewMatrix[1] + inverseOffset[1] * viewMatrix[5] + inverseOffset[0] * viewMatrix[9]);
	viewMatrix[14] = -(inverseOffset[2] * viewMatrix[2] + inverseOffset[1] * viewMatrix[6] + inverseOffset[0] * viewMatrix[10]);

	float spotMatrixTransposed[16];
	MatrixMultiply44(viewMatrix, projMatrix, spotMatrixTransposed);
	MatrixTranspose44(spotMatrixTransposed, g_lightInfo.spotMatrix);
}