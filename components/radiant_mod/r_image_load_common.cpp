#include "stdafx.h"

SRCLINE(101)
void Image_TrackTexture(GfxImage *image, int imageFlags, D3DFORMAT format, int width, int height, int depth)
{
	int depthUsed	= 0;
	int heightUsed	= 0;
	int widthUsed	= 0;

	for (int platform = 0; platform < 2; platform++)
	{
		if (imageFlags & 1)
		{
			widthUsed	= width;
			heightUsed	= height;
			depthUsed	= depth;
		}
		else
		{
			if ((width >> image->picmip.platform[platform]) > 1)
				widthUsed = width >> image->picmip.platform[platform];
			else
				widthUsed = 1;

			if ((height >> image->picmip.platform[platform]) > 1)
				heightUsed = height >> image->picmip.platform[platform];
			else
				heightUsed = 1;

			if ((depth >> image->picmip.platform[platform]) > 1)
				depthUsed = depth >> image->picmip.platform[platform];
			else
				depthUsed = 1;
		}

		int memory = Image_GetCardMemoryAmount(imageFlags, format, widthUsed, heightUsed, depthUsed);

		ASSERT(!image->cardMemory.platform[platform] || (image->cardMemory.platform[platform] == memory));

		// if (!useFastFile->current.enabled)
		//	Image_TrackTotalMemory(image, platform, memory);

		image->cardMemory.platform[platform] = memory;
	}
}

SRCLINE(145)
unsigned int Image_GetCardMemoryAmountForMipLevel(D3DFORMAT format, unsigned int mipWidth, unsigned int mipHeight, unsigned int mipDepth)
{
	switch (format)
	{
	case D3DFMT_DXT1:
		return 8 * mipDepth * ((mipHeight + 3) >> 2) * ((mipWidth + 3) >> 2);

	case D3DFMT_DXT3:
	case D3DFMT_DXT5:
		return 16 * mipDepth * ((mipHeight + 3) >> 2) * ((mipWidth + 3) >> 2);

	case D3DFMT_A16B16G16R16:
	case D3DFMT_A16B16G16R16F:
		return 8 * mipDepth * mipHeight * mipWidth;

	case D3DFMT_A8B8G8R8:
		return 4 * mipDepth * mipHeight * mipWidth;

	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_G16R16:
	case D3DFMT_D24S8:
	case D3DFMT_G16R16F:
	case D3DFMT_R32F:
		return 4 * mipDepth * mipHeight * mipWidth;

	case D3DFMT_R5G6B5:
	case D3DFMT_A8L8:
	case D3DFMT_D16:
		return 2 * mipDepth * mipHeight * mipWidth;

	case D3DFMT_A8:
	case D3DFMT_L8:
		return mipDepth * mipHeight * mipWidth;
	}

	ASSERT_MSG(false, "Unhandled case");
	return 0;
}

SRCLINE(278)
unsigned int Image_GetCardMemoryAmount(unsigned int imageFlags, D3DFORMAT format, unsigned int width, unsigned int height, unsigned int depth)
{
	unsigned int memory = Image_GetCardMemoryAmountForMipLevel(format, width, height, depth);

	if (!(imageFlags & 2))
	{
		while (depth + height + width > 3)
		{
			if ((signed int)(width >> 1) > 1)
				width = width >> 1;
			else
				width = 1;

			if ((signed int)(height >> 1) > 1)
				height = height >> 1;
			else
				height = 1;

			if ((signed int)(depth >> 1) > 1)
				depth = depth >> 1;
			else
				depth = 1;

			memory += Image_GetCardMemoryAmountForMipLevel(format, width, height, depth);
		}
	}

	if (imageFlags & 4)
		memory *= 6;

	return memory;
}

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
void Image_Setup(GfxImage *image, int width, int height, int depth, int imageFlags, D3DFORMAT imageFormat)
{
	image->width	= width;
	image->height	= height;
	image->depth	= depth;
	int mipmapCount = (imageFlags & 2) != 0;

	// r_loadForRenderer->current.enabled
	if (*(BYTE *)(*(DWORD *)0x191D574 + 12))
	{
		if (imageFlags & 4)
			Image_CreateCubeTexture_PC(image, image->width, mipmapCount, imageFormat);
		else if (imageFlags & 8)
			Image_Create3DTexture_PC(image, image->width, image->height, image->depth, mipmapCount, imageFlags, imageFormat);
		else
			Image_Create2DTexture_PC(image, image->width, image->height, mipmapCount, imageFlags, imageFormat);
		
		Image_TrackTexture(image, imageFlags, imageFormat, width, height, depth);

		// ASSERT(!image->delayLoadPixels);
	}
}

SRCLINE(1718)
D3DCUBEMAP_FACES Image_CubemapFace(unsigned int faceIndex)
{
	ASSERT(faceIndex < 6);

	return (D3DCUBEMAP_FACES)faceIndex;
}