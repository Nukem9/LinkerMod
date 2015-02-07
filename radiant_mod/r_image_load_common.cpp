#include "stdafx.h"

void Image_GetPicmip(GfxImage *image, Picmip *picmip)
{
	ASSERT(image);
	ASSERT(picmip);

	if (image->noPicmip)
		*&picmip->platform[0] = 0;
	else
		Image_PicmipForSemantic(image->semantic, picmip);
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
		*&picmip->platform[0] = 0;
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