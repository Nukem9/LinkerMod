#include "stdafx.h"

SRCLINE(673)
void Image_Upload2D_CopyData_PC(GfxImage *image, D3DFORMAT format, D3DCUBEMAP_FACES face, unsigned int mipLevel, const char *src)
{
	static DWORD dwCall = 0x006ABEC0;
	__asm
	{

		push src 
		push face
		push format
		push image

		mov edi, mipLevel
	
		call [dwCall]
		
		add esp, 0x10
	}
}

SRCLINE(824)
void Image_Upload3D_CopyData_PC(GfxImage *image, D3DFORMAT format, unsigned int mipLevel, const char *src)
{
	static DWORD dwCall = 0x006AC150;

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

	image->width = width;
	image->height = height;
	image->depth = depth;
	int mipmapCount = (imageFlags & 2) != 0;

	if ( imageFlags & 4 )
	{
		((void (__cdecl*)(GfxImage*, DWORD, DWORD, DWORD))0x006758A0)(image, image->width, mipmapCount, imageFormat);
	}
	else if ( imageFlags & 8 )
	{
		((void (__cdecl*)(GfxImage*, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD))0x00675770)(image, image->width, image->height, image->depth, mipmapCount, imageFlags, imageFormat);
		//Image_Create3DTexture_PC(image, image->width, image->height, image->depth, mipmapCount, imageFlags, imageFormat);
	}
	else
	{
		((void (__cdecl*)(GfxImage*, DWORD, DWORD, DWORD, DWORD, DWORD))0x00675640)(image, image->width, image->height, mipmapCount, imageFlags, imageFormat);
		//Image_Create2DTexture_PC(image, image->width, image->height, mipmapCount, imageFlags, imageFormat);
	}
	
	((void(__cdecl *)(GfxImage *, unsigned int, _D3DFORMAT, int, int, int))0x006AC470) //(Image TrackTexture?
		(image, imageFlags, imageFormat, width, height, depth);
}

SRCLINE(1718)
D3DCUBEMAP_FACES Image_CubemapFace(unsigned int faceIndex)
{
	ASSERT(faceIndex < 6);

	return (D3DCUBEMAP_FACES)faceIndex;
}