#include "stdafx.h"

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

	int bytesPerPixel = 4;
	switch (header->format)
	{
	case IMG_FORMAT_BITMAP_RGBA:
		image->hasAlpha = 1;
		Image_LoadBitmap(image, &dummyHeader, pixels, 4);
		break;
	case IMG_FORMAT_BITMAP_RGB:
		image->hasAlpha = 0;
		Image_LoadBitmap(image, &dummyHeader, pixels, 3);
		break;
	case IMG_FORMAT_BITMAP_LUMINANCE:
		image->hasAlpha = 0;
		Image_LoadBitmap(image, &dummyHeader, pixels, 1);
		break;
	case IMG_FORMAT_BITMAP_ALPHA:
		image->hasAlpha = 1;
		Image_LoadBitmap(image, &dummyHeader, pixels, 1);
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
		Image_LoadBitmap(image, &dummyHeader, pixels, 2);
		break;
	case IMG_FORMAT_BITMAP_LUMINANCE_ALPHA:
	case IMG_FORMAT_BITMAP_RGB5A3:
		image->hasAlpha = 1;
		Image_LoadBitmap(image, &dummyHeader, pixels, 2);
		break;
	default:
		Con_Error("Unhandled image type: %d (%s)\n", header->format, imageName);
		break;
	}

	FS_FreeFile(header);
}