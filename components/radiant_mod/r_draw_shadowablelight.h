#pragma once

enum GfxLightType
{
	GFX_LIGHT_TYPE_NONE = 0x0,
	GFX_LIGHT_TYPE_DIR = 0x1,
	GFX_LIGHT_TYPE_SPOT = 0x2,
	GFX_LIGHT_TYPE_OMNI = 0x3,
	GFX_LIGHT_TYPE_COUNT = 0x4,
	GFX_LIGHT_TYPE_DIR_SHADOWMAP = 0x4,
	GFX_LIGHT_TYPE_SPOT_SHADOWMAP = 0x5,
	GFX_LIGHT_TYPE_OMNI_SHADOWMAP = 0x6,
	GFX_LIGHT_TYPE_COUNT_WITH_SHADOWMAP_VERSIONS = 0x7,
};

struct GfxLight
{
	char type;
	char unk[3];
	float color[3];
	float angles[3];
	float origin[3];
	float radius;
	float cosHalfFovOuter;
	float cosHalfFovInner;
	int exponent;
};

struct GfxLightInfo
{
	GfxLightType type;

	vec4 aAbB;
	vec4 attenuation;

	vec4 coneControl1;
	vec4 coneControl2;

	vec4 fallOffA;
	vec4 fallOffB;

	float spotCookieControl[4];
	float spotFactors[4];

	float spotMatrix[16];

	struct
	{
		float roundness;

		float near_edge;
		float far_edge;

		float cut_on;
		float falloffdistance;

		vec3 angle;
		vec3 attenuation;

		vec4 superellipse;
	} kvs;
};

typedef int(__cdecl* R_SetLightProperties_t)(float* source, void *light, int def, int hasShadowMap, float spotShadowFade);
extern R_SetLightProperties_t R_SetLightProperties_o;

int __cdecl R_SetLightProperties(float* source, void *light, int def, int hasShadowMap, float spotShadowFade);

GfxLight* R_GetCurrentLight(void);
GfxLight* R_GetLastDirLight(void);
GfxLight* R_GetLastSpotLight(void);
GfxLight* R_GetLastOmniLight(void);

void SpotLightProjectionMatrix(float cosFov, float zNear, float zFar, float* mtx);
void SpotLightViewMatrix(const float *direction, float rotation, float* mtx);

extern GfxLightInfo g_lightInfo;
void R_BuildSpotLightInfo(float* source, GfxLight* light, float spotShadowFade);
