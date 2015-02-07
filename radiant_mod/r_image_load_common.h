#pragma once

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

struct Picmip
{
	char platform[2];
};

struct CardMemory
{
	int platform[2];
};

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
static_assert(sizeof(GfxImageFileHeader) == 48, "Invalid GfxImageFileHeader size!");

union GfxTexture
{
	int *ptr;
};

struct GfxImage
{
	int mapType;			// OK 0
	GfxTexture texture;		// OK 4
	Picmip picmip;			// OK 8
	bool noPicmip;			// OK 10
	char semantic;			// OK 11
	char track;				// OK 12

	char unk2[3];	// -- 12

	CardMemory cardMemory;	// OK 16
	unsigned short width;	// OK 24
	unsigned short height;	// OK 26
	unsigned short depth;	// OK 28
	char category;			// OK 30
	char _pad[1];	// -- 31
	const char *name;		// OK 32
};
/*
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
*/
static_assert(sizeof(GfxImage) == 0x24, "Invalid size for GfxImage");

void Image_GetPicmip(GfxImage *image, Picmip *picmip);
void Image_PicmipForSemantic(char semantic, Picmip *picmip);
D3DCUBEMAP_FACES Image_CubemapFace(unsigned int faceIndex);
void Image_Setup(GfxImage *image, int width, int height, int depth, unsigned int imageFlags, D3DFORMAT imageFormat);
void Image_Upload2D_CopyData_PC(GfxImage *image, D3DFORMAT format, D3DCUBEMAP_FACES face, unsigned int mipLevel, const char *src);
void Image_Upload3D_CopyData_PC(GfxImage *image, D3DFORMAT format, unsigned int mipLevel, const char *src);