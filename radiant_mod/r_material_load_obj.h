#pragma once

enum MaterialShaderType
{
	MTL_VERTEX_SHADER = 0x0,
	MTL_PIXEL_SHADER = 0x1,
};

enum ShaderParamType
{
	SHADER_PARAM_FLOAT4 = 0x0,
	SHADER_PARAM_SAMPLER_2D = 0x1,
	SHADER_PARAM_SAMPLER_3D = 0x2,
	SHADER_PARAM_SAMPLER_CUBE = 0x3,
	SHADER_PARAM_SAMPLER_1D = 0x4,
};
/*
enum MaterialTextureSource
{
	TEXTURE_SRC_CODE_BLACK = 0x0,
	TEXTURE_SRC_CODE_WHITE = 0x1,
	TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP = 0x2,
	TEXTURE_SRC_CODE_MODEL_LIGHTING = 0x3,
	TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY = 0x4,
	TEXTURE_SRC_CODE_LIGHTMAP_SECONDARY = 0x5,
	TEXTURE_SRC_CODE_SHADOWMAP_SUN = 0x6,
	TEXTURE_SRC_CODE_SHADOWMAP_SPOT = 0x7,
	TEXTURE_SRC_CODE_FEEDBACK = 0x8,
	TEXTURE_SRC_CODE_RESOLVED_POST_SUN = 0x9,
	TEXTURE_SRC_CODE_RESOLVED_SCENE = 0xA,
	TEXTURE_SRC_CODE_POST_EFFECT_SRC = 0xB,
	TEXTURE_SRC_CODE_POST_EFFECT_GODRAYS = 0xC,
	TEXTURE_SRC_CODE_POST_EFFECT_0 = 0xD,
	TEXTURE_SRC_CODE_POST_EFFECT_1 = 0xE,
	TEXTURE_SRC_CODE_SKY = 0xF,
	TEXTURE_SRC_CODE_LIGHT_ATTENUATION = 0x10,
	TEXTURE_SRC_CODE_DLIGHT_ATTENUATION = 0x11,
	TEXTURE_SRC_CODE_OUTDOOR = 0x12,
	TEXTURE_SRC_CODE_FLOATZ = 0x13,
	TEXTURE_SRC_CODE_PROCESSED_FLOATZ = 0x14,
	TEXTURE_SRC_CODE_RAW_FLOATZ = 0x15,
	TEXTURE_SRC_CODE_CASE_TEXTURE = 0x16,
	TEXTURE_SRC_CODE_CINEMATIC_Y = 0x17,
	TEXTURE_SRC_CODE_CINEMATIC_CR = 0x18,
	TEXTURE_SRC_CODE_CINEMATIC_CB = 0x19,
	TEXTURE_SRC_CODE_CINEMATIC_A = 0x1A,
	TEXTURE_SRC_CODE_REFLECTION_PROBE = 0x1B,
	TEXTURE_SRC_CODE_FEATHER_FLOAT_Z = 0x1C,
	TEXTURE_SRC_CODE_TERRAIN_SCORCH_TEXTURE_0 = 0x1D,
	TEXTURE_SRC_CODE_TERRAIN_SCORCH_TEXTURE_1 = 0x1E,
	TEXTURE_SRC_CODE_TERRAIN_SCORCH_TEXTURE_2 = 0x1F,
	TEXTURE_SRC_CODE_TERRAIN_SCORCH_TEXTURE_3 = 0x20,
	TEXTURE_SRC_CODE_TERRAIN_SCORCH_TEXTURE_LAST = 0x20,
	TEXTURE_SRC_CODE_LIGHTMAP_SECONDARYB = 0x21,
	TEXTURE_SRC_CODE_TEXTURE_0 = 0x22,
	TEXTURE_SRC_CODE_TEXTURE_1 = 0x23,
	TEXTURE_SRC_CODE_TEXTURE_2 = 0x24,
	TEXTURE_SRC_CODE_TEXTURE_3 = 0x25,
	TEXTURE_SRC_CODE_IMPACT_MASK = 0x26,
	TEXTURE_SRC_CODE_UI3D = 0x27,
	TEXTURE_SRC_CODE_MISSILE_CAM = 0x28,
	TEXTURE_SRC_CODE_COMPOSITE_RESULT = 0x29,
	TEXTURE_SRC_CODE_HEATMAP = 0x2A,
	TEXTURE_SRC_CODE_COUNT = 0x2B,
};

enum MaterialUpdateFrequency
{
	MTL_UPDATE_PER_PRIM = 0x0,
	MTL_UPDATE_PER_OBJECT = 0x1,
	MTL_UPDATE_RARELY = 0x2,
	MTL_UPDATE_CUSTOM = 0x3,
};
*/

typedef int MaterialUpdateFrequency;
typedef int MaterialTextureSource;

struct ShaderIndexRange
{
	unsigned int first;
	unsigned int count;
	bool isImplicit;
};

struct CodeSamplerSource
{
	const char *name;
	MaterialTextureSource source;
	CodeSamplerSource *subtable;
	int arrayCount;
	int arrayStride;
};

struct CodeConstantSource
{
	const char *name;
	unsigned char source;
	CodeConstantSource *subtable;
	int arrayCount;
	int arrayStride;
};

struct ShaderArgumentSource
{
	ShaderIndexRange indexRange;
	unsigned __int16 type;

	union
	{
		const float *literalConst;
		unsigned __int16 codeIndex;
		const char *name;
	} u;
};

bool Material_UsingTechnique(int techType);
bool Material_MatchToken(const char **text, const char *match);
bool Material_CopyTextToDXBuffer(void *cachedShader, unsigned int shaderLen, void **shader);
void *Material_LoadShader(const char *shaderName, const char *shaderVersion);
void *Material_RegisterTechnique(const char *name, int renderer);
bool Material_IgnoreTechnique(const char *name);
int Material_TechniqueTypeForName(const char *name);
void *Material_LoadTechniqueSet(const char *name, int renderer);

bool hk_Material_ParseSamplerSource();
bool hk_Material_ParseConstantSource();

bool hk_Material_DefaultArgumentSource();
void hk_Material_LoadShader();

extern const MaterialUpdateFrequency s_codeConstUpdateFreq[197];
extern const bool g_useTechnique[130];
extern CodeSamplerSource s_codeSamplers[];
extern CodeSamplerSource s_defaultCodeSamplers[];
extern CodeConstantSource s_codeConsts[];
extern CodeConstantSource s_defaultCodeConsts[];