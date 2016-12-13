#pragma once

union GfxTexture
{
	IDirect3DBaseTexture9 *basemap;
	IDirect3DTexture9 *map;
	IDirect3DVolumeTexture9 *volmap;
	IDirect3DCubeTexture9 *cubemap;
	void *loadDef;// GfxImageLoadDef
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

void Image_HandleMissingImage(int code, const char *fmt, const char* image);
unsigned int Image_GetUsage(int imageFlags, D3DFORMAT imageFormat);
void Image_Create2DTexture_PC(GfxImage *image, unsigned __int16 width, unsigned __int16 height, int mipmapCount, int imageFlags, D3DFORMAT imageFormat);
void Image_Create3DTexture_PC(GfxImage *image, unsigned __int16 width, unsigned __int16 height, unsigned __int16 depth, int mipmapCount, int imageFlags, D3DFORMAT imageFormat);
void Image_CreateCubeTexture_PC(GfxImage *image, unsigned __int16 edgeLen, int mipmapCount, D3DFORMAT imageFormat);

void hk_Image_Create2DTexture_PC(unsigned __int16 width, int mipmapCount, int imageFormat);
void hk_Image_Create3DTexture_PC(int image, int mipmapCount, int imageFormat);
void hk_Image_CreateCubeTexture_PC(signed int mipmapCount, int imageFormat);