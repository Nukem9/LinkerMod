#pragma once

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

void Image_GetPicmip(GfxImage *image, Picmip *picmip);
void Image_PicmipForSemantic(char semantic, Picmip *picmip);
unsigned int Image_CubemapFace(unsigned int faceIndex);