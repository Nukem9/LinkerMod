#include "stdafx.h"

bool SaveBitmap(char* filepath, BYTE* pixels, int width, int height, int bytesPerPixel) {
	FILE* f = NULL;
	fopen_s(&f, filepath, "wb");
	if (f == NULL)
	{
		Com_Printf("ERROR: Could not save image %s\n", filepath);
		return false;
	}

	BITMAPINFOHEADER info;
	ZeroMemory(&info, sizeof(info));
	info.biSize = sizeof(BITMAPINFOHEADER);
	info.biWidth = width;
	info.biHeight = height;
	info.biPlanes = 1;
	info.biBitCount = bytesPerPixel * 8;
	info.biCompression = BI_RGB;
	info.biSizeImage = width * height * bytesPerPixel;

	BITMAPFILEHEADER header;
	ZeroMemory(&header, sizeof(header));
	header.bfType = 'B' + ('M' << 8);
	header.bfOffBits = sizeof(BITMAPFILEHEADER) + info.biSize;
	header.bfSize = header.bfOffBits + info.biSizeImage;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;

	fwrite(&header, 1, sizeof(BITMAPFILEHEADER), f);
	fwrite(&info, 1, sizeof(BITMAPINFOHEADER), f);
	fwrite(pixels, 1, info.biSizeImage, f);
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
		{
			return 1;
		}
		else
		{
			Com_Printf("ERROR: image '%s' is version %i but should be version %i\n", filepath, imageFile->version, 13);
			return 0;
		}
	}

	Com_Printf("ERROR: image '%s' is not an IW image\n", filepath);
	return 0;
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

	// For Debugging
	sprintf_s(path, "dump/%s.bmp", imageName);
	Com_Printf("Loading image %dx%d (%d fmt)  %s\n", width, height, header->format, path);

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
		Image_LoadWavelet(image, &dummyHeader, pixels, 4);
		break;
	case IMG_FORMAT_WAVELET_RGB:
		image->hasAlpha = 0;
		Image_LoadWavelet(image, &dummyHeader, pixels, 3);
		break;
	case IMG_FORMAT_WAVELET_LUMINANCE_ALPHA:
		image->hasAlpha = 1;
		Image_LoadWavelet(image, &dummyHeader, pixels, 2);
		break;
	case IMG_FORMAT_WAVELET_LUMINANCE:
		image->hasAlpha = 0;
		Image_LoadWavelet(image, &dummyHeader, pixels, 1);
		break;
	case IMG_FORMAT_WAVELET_ALPHA:
		image->hasAlpha = 1;
		Image_LoadWavelet(image, &dummyHeader, pixels, 1);
		break;
	case IMG_FORMAT_DXT1:
		image->hasAlpha = 0;
		Image_LoadDxtc(image, &dummyHeader, pixels, 8);
		break;
	case IMG_FORMAT_DXT3:
	case IMG_FORMAT_DXT5:
		image->hasAlpha = 1;
		Image_LoadDxtc(image, &dummyHeader, pixels, 16);
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

void __cdecl Image_DecodeBitmap(struct GfxRawImage *image, struct t5::GfxImageFileHeader *imageFile, BYTE *pixels, int bytesPerPixel)
{
	ASSERT(image);
	ASSERT(imageFile);
	
	int faceCount = (imageFile->flags & 4) != 0 ? 6 : 1;
	int mipCount = Image_CountMipmapsForFile(imageFile) - 1;
	for (int mipLevel = mipCount; mipLevel >= 0; mipLevel--)
	{
		while (1)
		{
			int width = imageFile->dimensions[0] >> mipLevel;
			if (width <= 1)
				width = 1;
			int height = imageFile->dimensions[1] >> mipLevel;
			if (height <= 1)
				height = 1;
			
			int mipSize = bytesPerPixel * width * height;
			for (int faceIndex = 0; faceIndex < faceCount; faceIndex++)
			{
				if (faceIndex == 0 && mipLevel == 0)
					Image_CopyBitmapData(pixels, image, imageFile);
				pixels += mipSize;
			}
		}
	}
}