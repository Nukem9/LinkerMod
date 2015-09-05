#pragma once

struct MaterialConstantDefRaw
{
	unsigned int nameOffset;
	float literal[4];
};

struct MaterialTextureDefRaw
{
	unsigned int nameOffset;
	char samplerState;
	char semantic;
	union
	{
		unsigned int imageNameOffset;
		unsigned int waterDefOffset;
	};
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


static int gfxImageTextureTableSize		= 32768;
static GfxImage **gfxImageTextureTable	= (GfxImage **)0x01547F28;

void *Material_Alloc(int size);
void Material_ReloadTextures();