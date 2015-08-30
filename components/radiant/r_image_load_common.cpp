#include "stdafx.h"

SRCLINE(673)
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

SRCLINE(824)
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

SRCLINE(1080)
unsigned int Image_CountMipmaps(unsigned int imageFlags, unsigned int width, unsigned int height, unsigned int depth)
{
	if (imageFlags & 2)
		return 1;

	unsigned int mipCount	= 1;
	unsigned int mipRes		= 1;

	while (mipRes < width || mipRes < height || mipRes < depth)
	{
		mipRes *= 2;
		mipCount++;
	}

	return mipCount;
}

SRCLINE(1471)
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

SRCLINE(1497)
void Image_Setup(GfxImage *image, int width, int height, int depth, unsigned int imageFlags, D3DFORMAT imageFormat)
{
	switch (imageFlags)
	{
	case -64:
	case -63:
	case -61:
	case -112:
	case -48:
		imageFlags = 0;
		break;
	}

	((void(__cdecl *)(GfxImage *, int, int, int, unsigned int, D3DFORMAT))0x56AC60)
		(image, width, height, depth, imageFlags, imageFormat);
}

SRCLINE(1718)
D3DCUBEMAP_FACES Image_CubemapFace(unsigned int faceIndex)
{
	ASSERT(faceIndex < 6);

	return (D3DCUBEMAP_FACES)faceIndex;
}