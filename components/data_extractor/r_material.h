#pragma once

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

struct MaterialStreamRouting
{
	char source;
	char dest;
};

struct IDirect3DVertexDeclaration9
{
	void* baseclass_0;
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

struct IDirect3DVertexShader9
{
	void* baseclass_0;
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

struct IDirect3DPixelShader9
{
	void* baseclass_0;
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

struct MaterialShaderArgument
{
	unsigned __int16 type;
	unsigned __int16 dest;
	
	union MaterialArgumentDef
	{
		const float *literalConst;
		MaterialArgumentCodeConst codeConst;
		unsigned int codeSampler;
		unsigned int nameHash;
	};
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

struct IDirect3DResource9
{
	void* baseclass_0;
};

struct IDirect3DBaseTexture9
{
	IDirect3DResource9 baseclass_0;
};

struct IDirect3DTexture9
{
	IDirect3DBaseTexture9 baseclass_0;
};

struct IDirect3DVolumeTexture9
{
	IDirect3DBaseTexture9 baseclass_0;
};

struct IDirect3DCubeTexture9
{
	IDirect3DBaseTexture9 baseclass_0;
};

struct GfxImageLoadDef
{
	char levelCount;
	char flags;
	int format;
	int resourceSize;
	char data[1];
};

union GfxTexture
{
	IDirect3DBaseTexture9 *basemap;
	IDirect3DTexture9 *map;
	IDirect3DVolumeTexture9 *volmap;
	IDirect3DCubeTexture9 *cubemap;
	GfxImageLoadDef *loadDef;
};

struct Picmip
{
	char platform[2];
};

struct CardMemory
{
	int platform[2];
};

struct GfxImage
{
	GfxTexture texture;
	char mapType;
	char semantic;
	char category;
	bool delayLoadPixels;
	Picmip picmip;
	bool noPicmip;
	char track;
	CardMemory cardMemory;
	unsigned __int16 width;
	unsigned __int16 height;
	unsigned __int16 depth;
	char levelCount;
	char streaming;
	unsigned int baseSize;
	char *pixels;
	unsigned int loadedSize;
	char skippedMipLevels;
	const char *name;
	unsigned int hash;
};

struct WaterWritable
{
	float floatTime;
};

struct complex_s
{
	float real;
	float imag;
};

struct water_t
{
	WaterWritable writable;
	complex_s *H0;
	float *wTerm;
	int M;
	int N;
	float Lx;
	float Lz;
	float gravity;
	float windvel;
	float winddir[2];
	float amplitude;
	float codeConstant[4];
	GfxImage *image;
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
	
	union MaterialTextureDefInfo
	{
		GfxImage *image;
		water_t *water;
	};
};

struct MaterialConstantDef
{
	unsigned int nameHash;
	char name[12];
	float literal[4];
};

struct GfxStateBits
{
	unsigned int loadBits[2];
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
	
	union $C3F7A89422F64E7CE7B1D04043902CE5
	{
		MaterialConstantDef *localConstantTable;
		MaterialConstantDef *constantTable;
	};
	
	GfxStateBits *stateBitsTable;
};