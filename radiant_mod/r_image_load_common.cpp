#include "stdafx.h"

void Image_GetPicmip(GfxImage *image, Picmip *picmip)
{
	ASSERT(image != nullptr);
	ASSERT(picmip != nullptr);

	if (image->noPicmip)
	{
		picmip->platform[0] = 0;
		picmip->platform[1] = 0;
	}
	else
	{
		Image_PicmipForSemantic(image->semantic, picmip);
	}
}

void Image_PicmipForSemantic(char semantic, Picmip *picmip)
{
	signed int picmipUsed = 0;

	switch (semantic)
	{
	default:
		ASSERT(false && "Unhandled case");

	case 0:
	case 1:
		picmip->platform[0] = 0;
		picmip->platform[1] = 0;
		return;

	case 2:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
		picmipUsed = *(DWORD *)0x1567F28;
		break;

	case 5:
		picmipUsed = *(DWORD *)0x1567F2C;
		break;

	case 8:
		picmipUsed = *(DWORD *)0x1567F30;
		break;
	}

	picmip->platform[1] = 2;

	if (picmipUsed >= 0)
	{
		if (picmipUsed > 3)
			picmipUsed = 3;
	}
	else
	{
		picmipUsed = 0;
	}

	picmip->platform[0] = picmipUsed;
}

D3DCUBEMAP_FACES Image_CubemapFace(unsigned int faceIndex)
{
	ASSERT(faceIndex < 6);

	return (D3DCUBEMAP_FACES)faceIndex;
}

void Image_Setup(GfxImage *image, int width, int height, int depth, unsigned int imageFlags, D3DFORMAT imageFormat)
{
	((void(__cdecl *)(GfxImage *, int, int, int, unsigned int, D3DFORMAT))0x56AC60)
		(image, width, height, depth, imageFlags, imageFormat);
}

void Image_Upload2D_CopyData_PC(GfxImage *image, D3DFORMAT format, D3DCUBEMAP_FACES face, unsigned int mipLevel, const char *src)
{
	static DWORD dwCall = 0x56A6A0;

	__asm
	{
		push src
		push face
		push format
		mov edi, mipLevel
		mov eax, image
		call [dwCall]
		add esp, 0xC
	}
}

void Image_Upload3D_CopyData_PC(GfxImage *image, D3DFORMAT format, unsigned int mipLevel, const char *src)
{
	static DWORD dwCall = 0x56A8F0;

	__asm
	{
		push mipLevel
		push format
		push image
		mov eax, src
		call [dwCall]
		add esp, 0xC
	}
}