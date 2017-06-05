#pragma once

union GfxTexture
{
	int *ptr;
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
	int mapType;
	GfxTexture texture;
	Picmip picmip;
	bool noPicmip;
	char semantic;
	char track;
	char unk1[3];
	CardMemory cardMemory;
	unsigned __int16 width;
	unsigned __int16 height;
	unsigned __int16 depth;
	char category;
	char unk2[1];
	const char *name;
};

enum GfxRefBlendMode : DWORD
{
	BLENDMODE_OPAQUE = 0x0,
	BLENDMODE_BLEND = 0x1,
	BLENDMODE_GT0 = 0x2,
	BLENDMODE_GE128 = 0x3,
	BLENDMODE_LT128 = 0x4,
	BLENDMODE_ADD = 0x5,
};

struct GfxRawPixel
{
	char r;
	char g;
	char b;
	char a;
};

struct GfxRawImage
{
	char name[64];
	GfxRefBlendMode blendMode;
	bool hasAlpha;
	int width;
	int height;
	GfxRawPixel *pixels;
};

// Black Ops
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

typedef void (__cdecl* Image_LoadPixels_t)(struct GfxRawImage * image, struct GfxImageFileHeader * header, unsigned __int8 * pixels, int bitsPerPixel);
static Image_LoadPixels_t Image_LoadBitmap = (Image_LoadPixels_t)0x004176B0;
static Image_LoadPixels_t Image_LoadWavelet = (Image_LoadPixels_t)0x004178A0;
static Image_LoadPixels_t Image_LoadDxtc = (Image_LoadPixels_t)0x004177F0;

void hk_Image_GetRawPixels(void);
void __cdecl Image_GetRawPixels(GfxRawImage *image, const char *imageName);
