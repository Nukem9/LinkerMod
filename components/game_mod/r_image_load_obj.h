#pragma once

#define PICMIP_SIZES_STORED 8

enum
{
	MAPTYPE_NONE = 0x0,
	MAPTYPE_INVALID1 = 0x1,
	MAPTYPE_INVALID2 = 0x2,
	MAPTYPE_2D = 0x3,
	MAPTYPE_3D = 0x4,
	MAPTYPE_CUBE = 0x5,
	MAPTYPE_COUNT = 0x6,
};

enum GfxImageFileFormat
{
	IMG_FORMAT_INVALID = 0x0,
	IMG_FORMAT_BITMAP_RGBA = 0x1,
	IMG_FORMAT_BITMAP_RGB = 0x2,
	IMG_FORMAT_BITMAP_LUMINANCE_ALPHA = 0x3,
	IMG_FORMAT_BITMAP_LUMINANCE = 0x4,
	IMG_FORMAT_BITMAP_ALPHA = 0x5,
	IMG_FORMAT_WAVELET_RGBA = 0x6,
	IMG_FORMAT_WAVELET_RGB = 0x7,
	IMG_FORMAT_WAVELET_LUMINANCE_ALPHA = 0x8,
	IMG_FORMAT_WAVELET_LUMINANCE = 0x9,
	IMG_FORMAT_WAVELET_ALPHA = 0xA,
	IMG_FORMAT_DXT1 = 0xB,
	IMG_FORMAT_DXT3 = 0xC,
	IMG_FORMAT_DXT5 = 0xD,
	IMG_FORMAT_DXN = 0xE,
	IMG_FORMAT_BITMAP_RGB565 = 0xF,
	IMG_FORMAT_BITMAP_RGB5A3 = 0x10,
	IMG_FORMAT_BITMAP_C8 = 0x11,
	IMG_FORMAT_BITMAP_RGBA8 = 0x12,
	IMG_FORMAT_A16B16G16R16F = 0x13,
	IMG_FORMAT_COUNT = 0x14,
};

struct GfxImageFileHeader
{
	char tag[3];
	unsigned char version;
	unsigned char format;
	unsigned char flags;
	short dimensions[3];
	float gamma;
	int fileSizeForPicmip[PICMIP_SIZES_STORED];
};
STATIC_ASSERT_OFFSET(GfxImageFileHeader, dimensions, 0x6);
STATIC_ASSERT_OFFSET(GfxImageFileHeader, fileSizeForPicmip, 0x10);
STATIC_ASSERT_SIZE(GfxImageFileHeader, 0x30);

char Image_GetPcStreamedMips(GfxImageFileHeader *fileHeader);
bool Image_ValidateHeader(GfxImageFileHeader *imageFile, const char *filepath);
void Image_SetupFromFile(GfxImage *image, GfxImageFileHeader *fileHeader, D3DFORMAT imageFormat, unsigned int allocFlags);
int Image_CountMipmapsForFile(GfxImageFileHeader *fileHeader);
void Image_ExpandBgr(const char *src, unsigned int count, char *dst);
char *Image_AllocTempMemory(int bytes);
void Image_FreeTempMemory(char *mem, int bytes);
void Image_LoadWavelet(GfxImage *image, GfxImageFileHeader *fileHeader, const char *data, D3DFORMAT format, int bytesPerPixel, unsigned int allocFlags);
void Image_LoadBitmap(GfxImage *image, GfxImageFileHeader *fileHeader, char *data, D3DFORMAT format, int bytesPerPixel, int allocFlags);
void Image_LoadDxtc(GfxImage *image, GfxImageFileHeader *fileHeader, const char *data, D3DFORMAT format, int bytesPerBlock, unsigned int allocFlags);
void Image_LoadFromData(GfxImage *image, GfxImageFileHeader *fileHeader, char *srcData, unsigned int allocFlags);
void Image_PrintTruncatedFileError(const char *filepath);
bool Image_LoadToBuffer(GfxImage *image, bool loadHighmip, char **imageBuffer, int *bufferSize);