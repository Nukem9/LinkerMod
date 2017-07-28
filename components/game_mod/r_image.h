#pragma once

enum
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

enum
{
	PICMIP_PLATFORM_USED = 0x0,
	PICMIP_PLATFORM_MINSPEC = 0x1,
	PICMIP_PLATFORM_COUNT = 0x2,
};

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
	char platform[PICMIP_PLATFORM_COUNT];
};

struct CardMemory
{
	int platform[PICMIP_PLATFORM_COUNT];
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
STATIC_ASSERT_SIZE(GfxImage, 0x34);

static int& Image_TrackBytes = *(int *)0x045FC158;

void Image_HandleMissingImage(int code, const char *fmt, const char* image);
unsigned int Image_GetUsage(int imageFlags, D3DFORMAT imageFormat);
void Image_Create2DTexture_PC(GfxImage *image, unsigned __int16 width, unsigned __int16 height, int mipmapCount, int imageFlags, D3DFORMAT imageFormat);
void Image_Create3DTexture_PC(GfxImage *image, unsigned __int16 width, unsigned __int16 height, unsigned __int16 depth, int mipmapCount, int imageFlags, D3DFORMAT imageFormat);
void Image_CreateCubeTexture_PC(GfxImage *image, unsigned __int16 edgeLen, int mipmapCount, D3DFORMAT imageFormat);

void hk_Image_Create2DTexture_PC(unsigned __int16 width, int mipmapCount, int imageFormat);
void hk_Image_Create3DTexture_PC(int image, int mipmapCount, int imageFormat);
void hk_Image_CreateCubeTexture_PC(signed int mipmapCount, int imageFormat);

bool __cdecl Image_IsCodeImage(int track);
D3DFORMAT __cdecl R_ImagePixelFormat(GfxImage *image);
void __cdecl R_ImageList_f();
void Image_Release(GfxImage *image);
void Image_TrackTotalMemory(GfxImage *image, int platform, int memory);
void Image_PicmipForSemantic(char semantic, Picmip *picmip);