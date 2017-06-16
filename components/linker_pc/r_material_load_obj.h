#pragma once

#define MTL_GAMEFLAG_CASTS_SHADOW 0x40

struct GfxImage;
struct water_t;

struct MaterialStateMapRule
{
	unsigned int stateBitsMask[2];
	unsigned int stateBitsValue[2];
	unsigned int stateBitsSet[2];
	unsigned int stateBitsClear[2];
};

struct MaterialStateMapRuleSet
{
	int ruleCount;
	MaterialStateMapRule rules[1];
};

struct MaterialStateMap
{
	const char *name;
	MaterialStateMapRuleSet *ruleSet[10];
};

struct MaterialTechniqueSet
{
	const char *name;
	char worldVertFormat;
	char unused[1];
	unsigned __int16 techsetFlags;
	struct MaterialTechnique *techniques[130];
};

union MaterialTextureDefInfo
{
	GfxImage *image;
	water_t *water;
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

struct MaterialConstantDef
{
	unsigned int nameHash;
	char name[12];
	float _literal[4];
};

struct GfxStateBits
{
	unsigned int loadBits[2];
};

struct GfxDrawSurfFields
{
	__int64 _bf0;
};

union GfxDrawSurf
{
	GfxDrawSurfFields fields;
	unsigned __int64 packed;
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

#pragma pack(push, 1)
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
#pragma pack(pop)

struct MaterialTextureDefRaw
{
	unsigned int nameOffset;
	char samplerState;
	char semantic;

	union
	{
		unsigned int imageNameOffset;
		unsigned int waterDefOffset;
	} u;
};


struct MaterialConstantDefRaw
{
	unsigned int nameOffset;
	float literal[4];
};

struct MaterialInfoRaw
{
	unsigned int nameOffset;
	unsigned int refImageNameOffset;
	char gameFlags;
	char sortKey;
	char textureAtlasRowCount;
	char textureAtlasColumnCount;
	float maxDeformMove;
	char deformFlags;
	char usage;
	unsigned __int16 toolFlags;
	unsigned int locale;
	unsigned __int16 autoTexScaleWidth;
	unsigned __int16 autoTexScaleHeight;
	float tessSize;
	int surfaceFlags;
	int contents;
};

struct MaterialRaw
{
	MaterialInfoRaw info;
	unsigned int refStateBits[2];
	unsigned __int16 textureCount;
	unsigned __int16 constantCount;
	unsigned int techSetNameOffset;
	unsigned int textureTableOffset;
	unsigned int constantTableOffset;
};

struct GfxVertexShaderLoadDef
{
	unsigned int *program;
	unsigned __int16 programSize;
};

struct MaterialVertexShaderProgram
{
	void *vs;
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
	void *ps;
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
	struct MaterialVertexDeclaration *vertexDecl;
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

typedef bool(__cdecl* Material_HashStateMap_t)(const char *name, unsigned int *foundHashIndex);
static Material_HashStateMap_t Material_HashStateMap = (Material_HashStateMap_t)0x0047ECB0;

typedef MaterialStateMap *(__cdecl* Material_LoadStateMap_t)(const char *name);
static Material_LoadStateMap_t Material_LoadStateMap = (Material_LoadStateMap_t)0x0047F870;

typedef unsigned int (__cdecl* R_HashString_t)(const char *string);
static R_HashString_t R_HashString = (R_HashString_t)0x0048D200;

typedef GfxImage *(__cdecl* Image_Register_t)(const char *imageName, char semantic);
static Image_Register_t Image_Register = (Image_Register_t)0x0041B4E0;

typedef void* (__cdecl* Material_Alloc_t)(unsigned int size);
static Material_Alloc_t Material_Alloc = (Material_Alloc_t)0x004072F0;

typedef bool (__cdecl* Material_Validate_t)(Material *material);
static Material_Validate_t Material_Validate = (Material_Validate_t)0x00483B20;

typedef struct MaterialTechnique *(__cdecl* Material_GetTechnique_t)(Material *material, char techType);
static  Material_GetTechnique_t  Material_GetTechnique = (Material_GetTechnique_t)0x00470F20;

typedef void (__cdecl* Material_BuildStateBitsTable_t)(Material *material, unsigned int toolFlags, const unsigned int *refStateBits);
static Material_BuildStateBitsTable_t Material_BuildStateBitsTable = (Material_BuildStateBitsTable_t)0x004837D0;

typedef char (__cdecl* MaterialTexture_IsMatureContent_t)(const char texSemantic, const char *texImageName);
static MaterialTexture_IsMatureContent_t MaterialTexture_IsMatureContent = (MaterialTexture_IsMatureContent_t)0x00483DF0;

void hk_Material_RegisterStateMap(void);

Material *__cdecl Material_LoadRaw(MaterialRaw *mtlRaw, unsigned int materialType);
