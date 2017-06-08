#include "stdafx.h"
#include "r_image_wavelet.h"

#define IMAGE_DUMP	0
#define IMAGE_LOG	0

union ddscolor_t
{
	struct
	{
		unsigned __int16 b : 5;
		unsigned __int16 g : 6;
		unsigned __int16 r : 5;
	};

	unsigned __int16 rgb;
};
STATIC_ASSERT_SIZE(ddscolor_t, 2);

struct DdsBlock_Dxt1_t
{
	ddscolor_t color0;
	ddscolor_t color1;
	char bits[4];
};
STATIC_ASSERT_SIZE(DdsBlock_Dxt1_t, 8);

struct DdsBlock_Dxt3_t
{
	char alpha[8];
	DdsBlock_Dxt1_t color;
};
STATIC_ASSERT_SIZE(DdsBlock_Dxt3_t, 16);

struct DdsBlock_Dxt5_t
{
	char alpha0;
	char alpha1;
	char alpha[6];
	DdsBlock_Dxt1_t color;
};
STATIC_ASSERT_SIZE(DdsBlock_Dxt5_t, 16);

GfxRawPixel::GfxRawPixel(void)
{
}

GfxRawPixel::GfxRawPixel(vec3& vec)
{
	this->r = (BYTE)(vec.r * 255.0f);
	this->g = (BYTE)(vec.g * 255.0f);
	this->b = (BYTE)(vec.b * 255.0f);
	this->a = -1;
}

GfxRawPixel::GfxRawPixel(vec4& vec)
{
	this->r = (BYTE)(vec.r * 255.0f);
	this->g = (BYTE)(vec.g * 255.0f);
	this->b = (BYTE)(vec.b * 255.0f);
	this->a = (BYTE)(vec.a * 255.0f);
}

bool SaveBitmap(char* filepath, BYTE* pixels, int width, int height, int bytesPerPixel) {
	FILE* f = NULL;
	fopen_s(&f, filepath, "wb");
	if (f == NULL)
	{
		Com_Printf("ERROR: Could not save image %s\n", filepath);
		return false;
	}

	BITMAPV4HEADER info;
	ZeroMemory(&info, sizeof(info));
	info.bV4Size = sizeof(BITMAPV4HEADER);
	info.bV4Width = width;
	info.bV4Height = height;
	info.bV4Planes = 1;
	info.bV4BitCount = bytesPerPixel * 8;
	info.bV4V4Compression = BI_BITFIELDS;
	info.bV4SizeImage = width * height * bytesPerPixel;

	info.bV4RedMask		= 0x000000FF;
	info.bV4GreenMask	= 0x0000FF00;
	info.bV4BlueMask	= 0x00FF0000;
	if (bytesPerPixel == 4)
		info.bV4AlphaMask	= 0xFF000000;

	BITMAPFILEHEADER header;
	ZeroMemory(&header, sizeof(header));
	header.bfType = 'B' + ('M' << 8);
	header.bfOffBits = sizeof(BITMAPFILEHEADER) + info.bV4Size;
	header.bfSize = header.bfOffBits + info.bV4SizeImage;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;

	fwrite(&header, 1, sizeof(BITMAPFILEHEADER), f);
	fwrite(&info, 1, sizeof(BITMAPV4HEADER), f);
	fwrite(pixels, 1, info.bV4SizeImage, f);
	fclose(f);

	return true;
}

void __declspec(naked) hk_Image_GetRawPixels(void)
{
	_asm
	{
		push[esp+4] // imageName
		push esi // image
		call Image_GetRawPixels
		add esp, 8
		retn
	}
}

t4::GfxImageFileHeader::GfxImageFileHeader(t5::GfxImageFileHeader& header)
{
	tag[0] = header.tag[0];
	tag[1] = header.tag[1];
	tag[3] = header.tag[2];

	version = header.version;
	format = header.format;
	flags = header.flags;
	
	dimensions[0] = header.dimensions[0];
	dimensions[1] = header.dimensions[1];
	dimensions[2] = header.dimensions[2];

	fileSizeForPicmip[0] = header.fileSizeForPicmip[0];
	fileSizeForPicmip[1] = header.fileSizeForPicmip[1];
	fileSizeForPicmip[2] = header.fileSizeForPicmip[2];
	fileSizeForPicmip[3] = header.fileSizeForPicmip[3];
}

unsigned int Image_CountMipmaps(unsigned int imageFlags, unsigned int width, unsigned int height, unsigned int depth)
{
	if (imageFlags & 2)
		return 1;
	
	unsigned int mipCount = 1;
	unsigned int mipRes = 1;

	while (mipRes < width || mipRes < height || mipRes < depth)
	{
		mipRes *= 2;
		++mipCount;
	}

	return mipCount;
}

int Image_CountMipmapsForFile(t5::GfxImageFileHeader *fileHeader)
{
	return Image_CountMipmaps(
		fileHeader->flags,
		fileHeader->dimensions[0],
		fileHeader->dimensions[1],
		fileHeader->dimensions[2]);
}

bool __cdecl Image_ValidateHeader(t5::GfxImageFileHeader *imageFile, const char *filepath)
{
	if (imageFile->tag[0] == 'I' && imageFile->tag[1] == 'W' && imageFile->tag[2] == 'i')
	{
		if (imageFile->version == 13)
			return true;
		
		Com_Printf("ERROR: image '%s' is version %i but should be version %i\n", filepath, imageFile->version, 13);
		return false;
	}

	Com_Printf("ERROR: image '%s' is not an IW image\n", filepath);
	return false;
}

void __cdecl Image_GetRawPixels(GfxRawImage *image, const char *imageName)
{
	ASSERT(imageName != NULL);

	char path[MAX_PATH];
	Com_AssembleImageFilepath(imageName, path);
	
	t5::GfxImageFileHeader* header = NULL;
	if (FS_ReadFile(path, (void**)&header) < 0)
		Com_Error("image '%s' is missing", path);
	
	if (!Image_ValidateHeader(header, path))
		Com_Error("image '%s' is not valid", path);
	
	strcpy_s(image->name, imageName);

	int width = header->dimensions[0];
	image->width = width;
	int height = header->dimensions[1];
	image->height = height;
	image->pixels = (GfxRawPixel *)Z_Malloc(4 * height * width);

	BYTE* pixels = (PBYTE)&header[1];

	t4::GfxImageFileHeader dummyHeader(*header);

#if IMAGE_DUMP || IMAGE_LOG
	sprintf_s(path, "dump/%s.bmp", imageName);
	Com_Printf("Loading image %dx%d (%d fmt)  %s\n", width, height, header->format, path);
#endif

	switch (header->format)
	{
	case IMG_FORMAT_BITMAP_RGBA:
		image->hasAlpha = 1;
		Image_DecodeBitmap(image, header, pixels, 4);
		break;
	case IMG_FORMAT_BITMAP_RGB:
		image->hasAlpha = 0;
		Image_DecodeBitmap(image, header, pixels, 3);
		break;
	case IMG_FORMAT_BITMAP_LUMINANCE:
		image->hasAlpha = 0;
		Image_DecodeBitmap(image, header, pixels, 1);
		break;
	case IMG_FORMAT_BITMAP_ALPHA:
		image->hasAlpha = 1;
		Image_DecodeBitmap(image, header, pixels, 1);
		break;
	case IMG_FORMAT_WAVELET_RGBA:
		image->hasAlpha = 1;
		Image_DecodeWavelet(image, header, pixels, 4);
		break;
	case IMG_FORMAT_WAVELET_RGB:
		image->hasAlpha = 0;
		Image_DecodeWavelet(image, header, pixels, 3);
		break;
	case IMG_FORMAT_WAVELET_LUMINANCE_ALPHA:
		image->hasAlpha = 1;
		Image_DecodeWavelet(image, header, pixels, 2);
		break;
	case IMG_FORMAT_WAVELET_LUMINANCE:
		image->hasAlpha = 0;
		Image_DecodeWavelet(image, header, pixels, 1);
		break;
	case IMG_FORMAT_WAVELET_ALPHA:
		image->hasAlpha = 1;
		Image_DecodeWavelet(image, header, pixels, 1);
		break;
	case IMG_FORMAT_DXT1:
		image->hasAlpha = 0;
		Image_DecodeDxtc(image, header, pixels, 8);
		break;
	case IMG_FORMAT_DXT3:
	case IMG_FORMAT_DXT5:
		image->hasAlpha = 1;
		Image_DecodeDxtc(image, header, pixels, 16);
		break;
	case IMG_FORMAT_BITMAP_RGB565:
		image->hasAlpha = 0;
		Image_DecodeBitmap(image, header, pixels, 2);
		break;
	case IMG_FORMAT_BITMAP_LUMINANCE_ALPHA:
	case IMG_FORMAT_BITMAP_RGB5A3:
		image->hasAlpha = 1;
		Image_DecodeBitmap(image, header, pixels, 2);
		break;
	default:
		Con_Error("Unhandled image type: %d (%s)\n", header->format, imageName);
		break;
	}

#if IMAGE_DUMP
	SaveBitmap(path, (BYTE*)image->pixels, image->width, image->height, 4);
#endif
	FS_FreeFile(header);
}

void Image_CopyBitmapData(BYTE *pixels, GfxRawImage *image, t5::GfxImageFileHeader *imageFile)
{
	PBYTE src = pixels;
	GfxRawPixel* dst = image->pixels;
	
	int i = imageFile->dimensions[0] * imageFile->dimensions[1];
	
	switch (imageFile->format)
	{
	case IMG_FORMAT_BITMAP_RGBA:
	case IMG_FORMAT_WAVELET_RGBA:
		for (; i; --i)
		{
			dst->r = src[2];
			dst->g = src[1];
			dst->b = src[0];
			dst->a = src[3];
			src += 4;
			++dst;
		}
		break;
	case IMG_FORMAT_BITMAP_RGB:
	case IMG_FORMAT_WAVELET_RGB:
		for (; i; --i)
		{
			dst->r = src[2];
			dst->g = src[1];
			dst->b = src[0];
			dst->a = -1;
			src += 3;
			++dst;
		}
		break;
	case IMG_FORMAT_BITMAP_LUMINANCE_ALPHA:
	case IMG_FORMAT_WAVELET_LUMINANCE_ALPHA:
		for (; i; --i)
		{
			dst->r = src[0];
			dst->g = src[0];
			dst->b = src[0];
			dst->a = src[1];
			src += 2;
			++dst;
		}
		break;
	case IMG_FORMAT_BITMAP_LUMINANCE:
	case IMG_FORMAT_WAVELET_LUMINANCE:
		for (; i; --i)
		{
			dst->r = src[0];
			dst->g = src[0];
			dst->b = src[0];
			dst->a = -1;
			++src;
			++dst;
		}
		break;
	case IMG_FORMAT_BITMAP_ALPHA:
	case IMG_FORMAT_WAVELET_ALPHA:
		for (; i; --i)
		{
			dst->r = 0;
			dst->g = 0;
			dst->b = 0;
			dst->a = *src++;
			++dst;
		}
		break;
	case IMG_FORMAT_BITMAP_RGB565:
		for (; i; --i)
		{
			dst->r = (src[0] >> 5) | src[0] & 0xF8;
			dst->g = 32 * src[0] | ((BYTE)(src[0] & 6 | (src[1] >> 2) & 0x38) >> 1);
			dst->b = 8 * src[1] | (src[1] >> 2) & 7;
			dst->a = 0;
			++dst;
		}
		break;
	case IMG_FORMAT_BITMAP_RGB5A3:
		for (; i; --i)
		{
			BYTE r, g, b1, b2, a, v11, v12;
			if ((*src & 0x80u) == 0)
			{
				v11 = 16 * src[0];
				v12 = src[1];
				a = ((BYTE)(2 * (src[0] & 0xF0) | ((BYTE)(2 * (src[0] & 0xF0)) >> 3)) >> 3) | 2 * (src[0] & 0xF0);
				r = (v11 >> 4) | v11;
				g = ((BYTE)(v12 & 0xF0) >> 4) | v12 & 0xF0;
				b1 = 16 * v12;
				b2 = b1 >> 4;
			}
			else
			{
				BYTE _b = src[1];
				r = ((BYTE)(2 * (src[0] & 0xFC)) >> 5) | 2 * (src[0] & 0xFC);
				g = ((BYTE)((src[0] << 6) | (_b >> 2) & 0x38) >> 5) | (src[0] << 6) | (_b >> 2) & 0x38;
				b1 = 8 * _b;
				a = -1;
				b2 = (BYTE)(8 * _b) >> 5;
			}

			dst->g = g;
			dst->r = r;
			dst->b = b2 | b1;
			dst->a = a;
			++dst;
		}
		break;
	default:
		ASSERT_MSG(false, "unhandled case");
		break;
	}
}

void __cdecl Image_DecodeBitmap(struct GfxRawImage *image, struct t5::GfxImageFileHeader *imageFile, BYTE *data, int bytesPerPixel)
{
	ASSERT(image);
	ASSERT(imageFile);
	
	int faceCount = (imageFile->flags & t5::IMG_FLAG_CUBEMAP) != 0 ? 6 : 1;
	for (int mipLevel = Image_CountMipmapsForFile(imageFile) - 1; mipLevel >= 0; mipLevel--)
	{
		int width = max(imageFile->dimensions[0] >> mipLevel, 1);
		int height = max(imageFile->dimensions[1] >> mipLevel, 1);

		int mipSize = bytesPerPixel * width * height;

		for (int faceIndex = 0; faceIndex < faceCount; faceIndex++)
		{
			if (faceIndex == 0 && mipLevel == 0)
				Image_CopyBitmapData(data, image, imageFile);

			data += mipSize;
		}
	}
}

inline vec3 Pixel_Unpack_RGB565(unsigned __int16 packed)
{
	vec3 dst;
	dst.r = (packed >> 11) / 32.0f;
	dst.g = ((packed >> 5) & 0x3F) / 64.0f;
	dst.b = (packed & 0x1F) / 32.0f;
	return dst;
}

void Image_DecompressDxt1_Internal(GfxRawImage *image, DdsBlock_Dxt1_t *data, int x, int y, bool hasAlpha)
{
	vec3 color0 = Pixel_Unpack_RGB565(data->color0.rgb);
	vec3 color1 = Pixel_Unpack_RGB565(data->color1.rgb);

	GfxRawPixel rgba[4];

	if (hasAlpha || data->color0.rgb > data->color1.rgb)
	{
		rgba[0] = color0;
		rgba[1] = color1;

		rgba[2] = (color1 + (color0 * 2.0f)) / 3.0f;
		rgba[3] = (color0 + (color1 * 2.0f)) / 3.0f;
	}
	else
	{ 
		rgba[0] = color0;
		rgba[1] = color1;

		rgba[2] = (color0 + color1) / 2.0f;

		rgba[3].r = 0;
		rgba[3].g = 0;
		rgba[3].b = 0;
		rgba[3].a = 0;
	}

	image->Pixel(x + 0, y + 0) = rgba[(data->bits[0] >> 0) & 3];
	image->Pixel(x + 1, y + 0) = rgba[(data->bits[0] >> 2) & 3];
	image->Pixel(x + 2, y + 0) = rgba[(data->bits[0] >> 4) & 3];
	image->Pixel(x + 3, y + 0) = rgba[(data->bits[0] >> 6) & 3];

	image->Pixel(x + 0, y + 1) = rgba[(data->bits[1] >> 0) & 3];
	image->Pixel(x + 1, y + 1) = rgba[(data->bits[1] >> 2) & 3];
	image->Pixel(x + 2, y + 1) = rgba[(data->bits[1] >> 4) & 3];
	image->Pixel(x + 3, y + 1) = rgba[(data->bits[1] >> 6) & 3];

	image->Pixel(x + 0, y + 2) = rgba[(data->bits[2] >> 0) & 3];
	image->Pixel(x + 1, y + 2) = rgba[(data->bits[2] >> 2) & 3];
	image->Pixel(x + 2, y + 2) = rgba[(data->bits[2] >> 4) & 3];
	image->Pixel(x + 3, y + 2) = rgba[(data->bits[2] >> 6) & 3];

	image->Pixel(x + 0, y + 3) = rgba[(data->bits[3] >> 0) & 3];
	image->Pixel(x + 1, y + 3) = rgba[(data->bits[3] >> 2) & 3];
	image->Pixel(x + 2, y + 3) = rgba[(data->bits[3] >> 4) & 3];
	image->Pixel(x + 3, y + 3) = rgba[(data->bits[3] >> 6) & 3];
}

void Image_DecompressDxt1(DdsBlock_Dxt1_t* data, GfxRawImage *image, int x, int y)
{
	Image_DecompressDxt1_Internal(image, data, x, y, false);
}

void Image_DecompressDxt3(DdsBlock_Dxt3_t* data, GfxRawImage *image, int x, int y)
{
	Image_DecompressDxt1_Internal(image, &data->color, x, y, true);

	image->Pixel(x + 0, y + 0).a = 17 * (data->alpha[0] & 0xF);
	image->Pixel(x + 1, y + 0).a = 17 * (data->alpha[0] >> 4);
	image->Pixel(x + 2, y + 0).a = 17 * (data->alpha[1] & 0xF);
	image->Pixel(x + 3, y + 0).a = 17 * (data->alpha[1] >> 4);

	image->Pixel(x + 0, y + 1).a = 17 * (data->alpha[2] & 0xF);
	image->Pixel(x + 1, y + 1).a = 17 * (data->alpha[2] >> 4);
	image->Pixel(x + 2, y + 1).a = 17 * (data->alpha[3] & 0xF);
	image->Pixel(x + 3, y + 1).a = 17 * (data->alpha[3] >> 4);

	image->Pixel(x + 0, y + 2).a = 17 * (data->alpha[4] & 0xF);
	image->Pixel(x + 1, y + 2).a = 17 * (data->alpha[4] >> 4);
	image->Pixel(x + 2, y + 2).a = 17 * (data->alpha[5] & 0xF);
	image->Pixel(x + 3, y + 2).a = 17 * (data->alpha[5] >> 4);

	image->Pixel(x + 0, y + 3).a = 17 * (data->alpha[6] & 0xF);
	image->Pixel(x + 1, y + 3).a = 17 * (data->alpha[6] >> 4);
	image->Pixel(x + 2, y + 3).a = 17 * (data->alpha[7] & 0xF);
	image->Pixel(x + 3, y + 3).a = 17 * (data->alpha[7] >> 4);
}

void __cdecl Image_DecompressDxt5(DdsBlock_Dxt5_t *data, GfxRawImage *image, int x, int y)
{
	Image_DecompressDxt1_Internal(image, &data->color, x, y, true);

	BYTE alpha[8];
	alpha[0] = data->alpha0;
	alpha[1] = data->alpha1;

	int alpha0 = (BYTE)data->alpha0;
	int alpha1 = (BYTE)data->alpha1;

	// Calculate the remaining alpha values
	if (alpha[0] > alpha[1])
	{
		alpha[2] = (BYTE)((6 * alpha0 + 1 * alpha1) / 7);
		alpha[3] = (BYTE)((5 * alpha0 + 2 * alpha1) / 7);
		alpha[4] = (BYTE)((4 * alpha0 + 3 * alpha1) / 7);
		alpha[5] = (BYTE)((3 * alpha0 + 4 * alpha1) / 7);
		alpha[6] = (BYTE)((2 * alpha0 + 5 * alpha1) / 7);
		alpha[7] = (BYTE)((1 * alpha0 + 6 * alpha1) / 7);
	}
	else
	{
		alpha[2] = (BYTE)((4 * alpha0 + 1 * alpha1) / 5);
		alpha[3] = (BYTE)((3 * alpha0 + 2 * alpha1) / 5);
		alpha[4] = (BYTE)((2 * alpha0 + 3 * alpha1) / 5);
		alpha[5] = (BYTE)((1 * alpha0 + 4 * alpha1) / 5);
		alpha[6] = 0;
		alpha[7] = 255;
	}

	// Calculate the alpha lookup indices
	BYTE indices[16];
	for (int i = 0; i < 2; i++)
	{
		int value = 0;
		// Load 3 bytes into 'value'
		for (int j = 0; j < 3; j++)
			value |= ((BYTE)data->alpha[i * 3 + j] << 8 * j);

		// Unpack 8 3-bit indices from the loaded bytes
		for (int j = 0; j < 8; j++)
			indices[i * 8 + j] = (BYTE)((value >> 3 * j) & 7);
	}

	// Apply the alpha values to their respective pixels
	image->Pixel(x + 0, y + 0).a = alpha[indices[0]];
	image->Pixel(x + 1, y + 0).a = alpha[indices[1]];
	image->Pixel(x + 2, y + 0).a = alpha[indices[2]];
	image->Pixel(x + 3, y + 0).a = alpha[indices[3]];

	image->Pixel(x + 0, y + 1).a = alpha[indices[4]];
	image->Pixel(x + 1, y + 1).a = alpha[indices[5]];
	image->Pixel(x + 2, y + 1).a = alpha[indices[6]];
	image->Pixel(x + 3, y + 1).a = alpha[indices[7]];

	image->Pixel(x + 0, y + 2).a = alpha[indices[8]];
	image->Pixel(x + 1, y + 2).a = alpha[indices[9]];
	image->Pixel(x + 2, y + 2).a = alpha[indices[10]];
	image->Pixel(x + 3, y + 2).a = alpha[indices[11]];

	image->Pixel(x + 0, y + 3).a = alpha[indices[12]];
	image->Pixel(x + 1, y + 3).a = alpha[indices[13]];
	image->Pixel(x + 2, y + 3).a = alpha[indices[14]];
	image->Pixel(x + 3, y + 3).a = alpha[indices[15]];
}

void Image_CopyDxtcData(BYTE *data, GfxRawImage *image, t5::GfxImageFileHeader *imageFile)
{
	typedef void(*Image_DecompressDxtcBlock_t)(void* data, GfxRawImage *image, int x, int y);
	Image_DecompressDxtcBlock_t Image_DecompressDxtcBlock = NULL;
	int bytesPerPixel = 1;

	switch (imageFile->format)
	{
	case IMG_FORMAT_DXT1:
		Image_DecompressDxtcBlock = (Image_DecompressDxtcBlock_t)Image_DecompressDxt1;
		bytesPerPixel = 8;
		break;
	case IMG_FORMAT_DXT3:
		Image_DecompressDxtcBlock = (Image_DecompressDxtcBlock_t)Image_DecompressDxt3;
		bytesPerPixel = 16;
		break;
	case IMG_FORMAT_DXT5:
		Image_DecompressDxtcBlock = (Image_DecompressDxtcBlock_t)Image_DecompressDxt5;
		bytesPerPixel = 16;
		break;
	default:
		Com_Error("unhandled case - unsupported image format for file '%s'", image->name);
		return;
	}

	// Size of DXT compressed block in pixels
	const int DXT_BLOCK_WIDTH = 4;
	const int DXT_BLOCK_HEIGHT = 4;

	for (int y = 0; y < image->height; y += DXT_BLOCK_WIDTH)
	{
		for (int x = 0; x < image->width; x += DXT_BLOCK_HEIGHT)
		{
			Image_DecompressDxtcBlock(data, image, x, y);
			data += bytesPerPixel;
		}
	}
}

void __cdecl Image_DecodeDxtc(struct GfxRawImage *image, struct t5::GfxImageFileHeader *imageFile, BYTE *data, int bytesPerPixel)
{
	ASSERT(image);
	ASSERT(imageFile);

	int faceCount = (imageFile->flags & t5::IMG_FLAG_CUBEMAP) != 0 ? 6 : 1;
	for (int mipLevel = Image_CountMipmapsForFile(imageFile) - 1; mipLevel >= 0; mipLevel--)
	{
		int width = max(imageFile->dimensions[0] >> mipLevel, 1);
		int height = max(imageFile->dimensions[1] >> mipLevel, 1);

		int mipSize = bytesPerPixel * ((width + 3) >> 2) * ((height + 3) >> 2);

		for (int faceIndex = 0; faceIndex < faceCount; faceIndex++)
		{
			if (faceIndex == 0 && mipLevel == 0)
				Image_CopyDxtcData(data, image, imageFile);

			data += mipSize;
		}
	}
}