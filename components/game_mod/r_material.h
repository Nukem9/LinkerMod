#pragma once
#include "r_image.h"

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
	float literal[4];
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
