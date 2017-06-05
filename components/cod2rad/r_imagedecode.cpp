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

bool __cdecl Image_ValidateHeader(GfxImageFileHeader *imageFile, const char *filepath)
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
	
	GfxImageFileHeader* header = NULL;
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

	int bytesPerPixel = 4;
	switch (header->format)
	{
	case 1:
		image->hasAlpha = 1;
		Image_LoadBitmap(image, header, (PBYTE)&header[1], 4);
		break;
	case 2:
		image->hasAlpha = 0;
		Image_LoadBitmap(image, header, (PBYTE)&header[1], 3);
		break;
	case 4:
		image->hasAlpha = 0;
		Image_LoadBitmap(image, header, (PBYTE)&header[1], 1);
		break;
	case 5:
		image->hasAlpha = 1;
		Image_LoadBitmap(image, header, (PBYTE)&header[1], 1);
		break;
	case 6:
		image->hasAlpha = 1;
		Image_LoadWavelet(image, header, (PBYTE)&header[1], 4);
		break;
	case 7:
		image->hasAlpha = 0;
		Image_LoadWavelet(image, header, (PBYTE)&header[1], 3);
		break;
	case 8:
		image->hasAlpha = 1;
		Image_LoadWavelet(image, header, (PBYTE)&header[1], 2);
		break;
	case 9:
		image->hasAlpha = 0;
		Image_LoadWavelet(image, header, (PBYTE)&header[1], 1);
		break;
	case 10:
		image->hasAlpha = 1;
		Image_LoadWavelet(image, header, (PBYTE)&header[1], 1);
		break;
	case 11:
		image->hasAlpha = 0;
		Image_LoadDxtc(image, header, (PBYTE)&header[1], 8);
		break;
	case 12:
	case 13:
		image->hasAlpha = 1;
		Image_LoadDxtc(image, header, (PBYTE)&header[1], 16);
		break;
	case 15:
		image->hasAlpha = 0;
		Image_LoadBitmap(image, header, (PBYTE)&header[1], 2);
		break;
	case 3:
	case 16:
		image->hasAlpha = 1;
		Image_LoadBitmap(image, header, (PBYTE)&header[1], 2);
		break;
	default:
		Con_Error("Unhandled image type: %d (%s)\n", header->format, imageName);
		break;
	}

	FS_FreeFile(header);
}