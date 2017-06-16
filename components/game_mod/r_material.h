#pragma once
#include "r_image.h"

struct GfxDrawSurfFields
{
	__int64 _bf0;
};

union GfxDrawSurf
{
	GfxDrawSurfFields fields;
	unsigned __int64 packed;
};

struct GfxStateBits
{
	unsigned int loadBits[2];
};

struct MaterialInfo
{
	const char *name;
	unsigned int gameFlags;
	char pad;
	char sortKey;
	char textureAtlasRowCount;
	char textureAtlasColumnCount;
	GfxDrawSurf drawSurf;
	unsigned int surfaceTypeBits;
	unsigned int layeredSurfaceTypes;
	unsigned __int16 hashIndex;
};

struct MaterialStreamRouting
{
	char source;
	char dest;
};

struct MaterialVertexStreamRouting
{
	MaterialStreamRouting data[16];
	IDirect3DVertexDeclaration9 *decl[18];
};

struct MaterialVertexDeclaration
{
	char streamCount;
	bool hasOptionalSource;
	bool isLoaded;
	MaterialVertexStreamRouting routing;
};

enum MaterialVertexDeclType
{
	VERTDECL_GENERIC = 0x0,
	VERTDECL_PACKED = 0x1,
	VERTDECL_WORLD = 0x2,
	VERTDECL_WORLD_T1N0 = 0x3,
	VERTDECL_WORLD_T1N1 = 0x4,
	VERTDECL_WORLD_T2N0 = 0x5,
	VERTDECL_WORLD_T2N1 = 0x6,
	VERTDECL_WORLD_T2N2 = 0x7,
	VERTDECL_WORLD_T3N0 = 0x8,
	VERTDECL_WORLD_T3N1 = 0x9,
	VERTDECL_WORLD_T3N2 = 0xA,
	VERTDECL_WORLD_T4N0 = 0xB,
	VERTDECL_WORLD_T4N1 = 0xC,
	VERTDECL_WORLD_T4N2 = 0xD,
	VERTDECL_POS_TEX = 0xE,
	VERTDECL_STATICMODELCACHE = 0xF,
	VERTDECL_WATER = 0x10,
	VERTDECL_PARTICLECLOUD = 0x11,
	VERTDECL_COUNT = 0x12,
};

struct GfxVertexShaderLoadDef
{
	unsigned int *program;
	unsigned __int16 programSize;
};

struct MaterialVertexShaderProgram
{
	IDirect3DVertexShader9 *vs;
	GfxVertexShaderLoadDef loadDef;
};

struct MaterialVertexShader
{
	const char *name;
	MaterialVertexShaderProgram prog;
};

struct GfxPixelShaderLoadDef
{
	unsigned int *program;
	unsigned __int16 programSize;
};

struct MaterialPixelShaderProgram
{
	IDirect3DPixelShader9 *ps;
	GfxPixelShaderLoadDef loadDef;
};

struct MaterialPixelShader
{
	const char *name;
	MaterialPixelShaderProgram prog;
};

struct MaterialArgumentCodeConst
{
	unsigned __int16 index;
	char firstRow;
	char rowCount;
};

union MaterialArgumentDef
{
	const float *literalConst;
	MaterialArgumentCodeConst codeConst;
	unsigned int codeSampler;
	unsigned int nameHash;
};

struct MaterialShaderArgument
{
	unsigned __int16 type;
	unsigned __int16 dest;
	MaterialArgumentDef u;
};

struct MaterialPass
{
	MaterialVertexDeclaration *vertexDecl;
	MaterialVertexShader *vertexShader;

	union
	{
		MaterialPixelShader *pixelShader;
		MaterialPixelShader *localPixelShader;
	};

	char perPrimArgCount;
	char perObjArgCount;
	char stableArgCount;
	char customSamplerFlags;

	union
	{
		MaterialShaderArgument *localArgs;
		MaterialShaderArgument *args;
	};
};

struct MaterialTechnique
{
	const char *name;
	unsigned __int16 flags;
	unsigned __int16 passCount;
	MaterialPass passArray[1];
};

struct MaterialTechniqueSet
{
	const char *name;
	char worldVertFormat;
	char unused[1];
	unsigned __int16 techsetFlags;
	MaterialTechnique *techniques[130];
};

struct MaterialConstantDef
{
	unsigned int nameHash;
	char name[12];
	float _literal[4];
};

union MaterialTextureDefInfo
{
	GfxImage *image;
	//water_t *water;
};

struct MaterialTextureDef
{
	unsigned int nameHash;
	char nameStart;
	char nameEnd;
	char samplerState;
	char semantic;
	char isMatureContent;
	char pad[3];
	MaterialTextureDefInfo u;
};

struct Material
{
	MaterialInfo info;
	char stateBitsEntry[130];
	char textureCount;
	char constantCount;
	char stateBitsCount;
	char stateFlags;
	char cameraRegion;
	char maxStreamedMips;

	union
	{
		MaterialTechniqueSet *localTechniqueSet;
		MaterialTechniqueSet *techniqueSet;
	};

	MaterialTextureDef *textureTable;
	
	union
	{
		MaterialConstantDef *localConstantTable;
		MaterialConstantDef *constantTable;
	};

	GfxStateBits *stateBitsTable;
};

void __cdecl R_MaterialList_f();

MaterialTechniqueSet *__cdecl Material_GetTechniqueSet(Material *material);
MaterialTechnique *__cdecl Material_GetTechnique(Material *material, char techType);
