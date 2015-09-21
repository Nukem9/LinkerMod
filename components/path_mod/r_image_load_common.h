#pragma once

#define PICMIP_PLATFORM_USED 0

enum MapType
{
	MAPTYPE_NONE = 0x0,
	MAPTYPE_INVALID1 = 0x1,
	MAPTYPE_INVALID2 = 0x2,
	MAPTYPE_2D = 0x3,
	MAPTYPE_3D = 0x4,
	MAPTYPE_CUBE = 0x5,
	MAPTYPE_COUNT = 0x6,
};

enum ImageCategory
{
	IMG_CATEGORY_UNKNOWN = 0x0,
	IMG_CATEGORY_AUTO_GENERATED = 0x1,
	IMG_CATEGORY_LIGHTMAP = 0x2,
	IMG_CATEGORY_LOAD_FROM_FILE = 0x3,
	IMG_CATEGORY_RAW = 0x4,
	IMG_CATEGORY_FIRST_UNMANAGED = 0x5,
	IMG_CATEGORY_WATER = 0x5,
	IMG_CATEGORY_RENDERTARGET = 0x6,
	IMG_CATEGORY_TEMP = 0x7,
};

struct Picmip
{
	char platform[2];
};
CHECK_SIZE(Picmip, 2);

struct CardMemory
{
	int platform[2];
};
CHECK_SIZE(CardMemory, 8);

struct GfxImageFileHeader
{
	char tag[3];
	char version;
	char format;
	char flags;
	__int16 dimensions[3];
	float gamma;
	int fileSizeForPicmip[8];
};
CHECK_SIZE(GfxImageFileHeader, 48);

union GfxTexture
{
	int *ptr;
};
CHECK_SIZE(GfxTexture, 4);

struct GfxImage
{
	int mapType;
	GfxTexture texture;
	Picmip picmip;
	bool noPicmip;
	char semantic;
	char track;
	char unk1[3];
	CardMemory cardMemory;
	unsigned short width;
	unsigned short height;
	unsigned short depth;
	char category;
	char unk2[1];
	const char *name;
};
CHECK_SIZE(GfxImage, 36);


void Image_Upload2D_CopyData_PC(GfxImage *image, D3DFORMAT format, D3DCUBEMAP_FACES face, unsigned int mipLevel, const char *src);
void Image_Upload3D_CopyData_PC(GfxImage *image, D3DFORMAT format, unsigned int mipLevel, const char *src);
unsigned int Image_CountMipmaps(unsigned int imageFlags, unsigned int width, unsigned int height, unsigned int depth);
void Image_GetPicmip(GfxImage *image, Picmip *picmip);
void Image_Setup(GfxImage *image, int width, int height, int depth, unsigned int imageFlags, D3DFORMAT imageFormat);
D3DCUBEMAP_FACES Image_CubemapFace(unsigned int faceIndex);