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

	GfxRawPixel(void);
	GfxRawPixel(vec3& vec);
	GfxRawPixel(vec4& vec);
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
STATIC_ASSERT_SIZE(GfxRawImage, 0x54);

namespace t5
{
	// Black Ops image file header
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
}

namespace t4
{
	// World at War image file header
	struct GfxImageFileHeader
	{
		char tag[3];
		char version;
		char format;
		char flags;
		__int16 dimensions[3];
		int fileSizeForPicmip[4];

		GfxImageFileHeader(t5::GfxImageFileHeader& header);
	};
}

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

typedef void (__cdecl* Image_LoadPixels_t)(struct GfxRawImage * image, struct t4::GfxImageFileHeader * header, unsigned __int8 * pixels, int bitsPerPixel);
//static Image_LoadPixels_t Image_DecodeBitmap = (Image_LoadPixels_t)0x004176B0;
//static Image_LoadPixels_t Image_DecodeWavelet = (Image_LoadPixels_t)0x004178A0;
//static Image_LoadPixels_t Image_DecodeDxtc = (Image_LoadPixels_t)0x004177F0;

int Image_CountMipmapsForFile(t5::GfxImageFileHeader *fileHeader);

void hk_Image_GetRawPixels(void);
void __cdecl Image_GetRawPixels(GfxRawImage *image, const char *imageName);

void Image_CopyBitmapData(BYTE *data, GfxRawImage *image, t5::GfxImageFileHeader *imageFile);
void __cdecl Image_DecodeBitmap(struct GfxRawImage *image, struct t5::GfxImageFileHeader *imageFile, BYTE *data, int bytesPerPixel);
void __cdecl Image_DecodeDxtc(struct GfxRawImage *image, struct t5::GfxImageFileHeader *imageFile, BYTE *data, int bytesPerPixel);