#include "stdafx.h"

// /gfx_d3d/r_image_load_common.cpp:101
void Image_TrackTexture(GfxImage *image, int imageFlags, D3DFORMAT format, int width, int height, int depth)
{
	int depthUsed = 0;
	int heightUsed = 0;
	int widthUsed = 0;

	for (int platform = 0; platform < PICMIP_PLATFORM_COUNT; platform++)
	{
		if (imageFlags & 1)
		{
			widthUsed = width;
			heightUsed = height;
			depthUsed = depth;
		}
		else
		{
			widthUsed = max(1, width >> image->picmip.platform[platform]);
			heightUsed = max(1, height >> image->picmip.platform[platform]);
			depthUsed = max(1, depth >> image->picmip.platform[platform]);
		}

		int memory = Image_GetCardMemoryAmount(imageFlags, format, widthUsed, heightUsed, depthUsed);

		ASSERT(!image->cardMemory.platform[platform] || (image->cardMemory.platform[platform] == memory));

		if (!useFastFile->current.enabled)
			Image_TrackTotalMemory(image, platform, memory);

		image->cardMemory.platform[platform] = memory;
	}
}

// /gfx_d3d/r_image_load_common.cpp:145
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

	ASSERT_MSG_VA(false, "Unhandled case %d", format);
	return 0;
}

// /gfx_d3d/r_image_load_common.cpp:278
unsigned int Image_GetCardMemoryAmount(unsigned int imageFlags, D3DFORMAT format, unsigned int width, unsigned int height, unsigned int depth)
{
	unsigned int memory = Image_GetCardMemoryAmountForMipLevel(format, width, height, depth);

	if (!(imageFlags & 2))
	{
		while (depth + height + width > 3)
		{
			width = max(1, width >> 1);
			height = max(1, height >> 1);
			depth = max(1, depth >> 1);

			memory += Image_GetCardMemoryAmountForMipLevel(format, width, height, depth);
		}
	}

	// Cube texture multiplier
	if (imageFlags & 4)
		memory *= 6;

	return memory;
}

// /gfx_d3d/r_image_load_common.cpp:673
void Image_Upload2D_CopyData_PC(GfxImage *image, D3DFORMAT format, D3DCUBEMAP_FACES face, unsigned int mipLevel, const char *src)
{
	static DWORD dwCall = 0x00739840;

	__asm
	{
		push src
		push face
		push format
		mov esi, mipLevel
		mov eax, image
		call [dwCall]
		add esp, 0xC
	}
}

// /gfx_d3d/r_image_load_common.cpp:824
void Image_Upload3D_CopyData_PC(GfxImage *image, D3DFORMAT format, unsigned int mipLevel, const char *src)
{
	((void(__cdecl *)(GfxImage *, D3DFORMAT, unsigned int, const char *))0x00739910)(image, format, mipLevel, src);
}

// /gfx_d3d/r_image_load_common.cpp:1039
void Image_UploadData(GfxImage *image, D3DFORMAT format, D3DCUBEMAP_FACES face, unsigned int mipLevel, const char *src)
{
	if (image->mapType != MAPTYPE_CUBE || !mipLevel || *(bool *)0x396A4F7)
	{
		if (image->mapType == MAPTYPE_3D)
			Image_Upload3D_CopyData_PC(image, format, mipLevel, src);
		else
			Image_Upload2D_CopyData_PC(image, format, face, mipLevel, src);
	}
}

// /gfx_d3d/r_image_load_common.cpp:1080
unsigned int Image_CountMipmaps(unsigned int imageFlags, unsigned int width, unsigned int height, unsigned int depth)
{
	if (imageFlags & 2)
		return 1;

	unsigned int mipCount = 1;
	unsigned int mipRes = 1;

	while (mipRes < width || mipRes < height || mipRes < depth)
	{
		mipRes *= 2;
		mipCount++;
	}

	return mipCount;
}

// /gfx_d3d/r_image_load_common.cpp:1471
void Image_GetPicmip(GfxImage *image, Picmip *picmip)
{
	ASSERT(image);
	ASSERT(picmip);

	if (image->noPicmip)
	{
		picmip->platform[PICMIP_PLATFORM_USED] = 0;
		picmip->platform[PICMIP_PLATFORM_MINSPEC] = 0;
		return;
	}

	Image_PicmipForSemantic(image->semantic, picmip);
}

// /gfx_d3d/r_image_load_common.cpp:1497
void Image_Setup(GfxImage *image, int width, int height, int depth, char imageFlags, D3DFORMAT imageFormat, const char *name, unsigned int allocFlags)
{
	ASSERT(image);

	image->width = width;
	image->height = height;
	image->depth = depth;
	int mipmapCount = (imageFlags & 2) != 0;

	if (!r_loadForRenderer->current.enabled)
		return;

	// Renderer is enabled, create D3D image buffer
	if (imageFlags & 4)
		Image_CreateCubeTexture_PC(image, image->width, mipmapCount, imageFormat);
	else if (imageFlags & 8)
		Image_Create3DTexture_PC(image, image->width, image->height, image->depth, mipmapCount, imageFlags, imageFormat);
	else
		Image_Create2DTexture_PC(image, image->width, image->height, mipmapCount, imageFlags, imageFormat);

	Image_TrackTexture(image, imageFlags, imageFormat, width, height, depth);

	ASSERT(!image->delayLoadPixels);
}

// /gfx_d3d/r_image_load_common.cpp:1718
D3DCUBEMAP_FACES Image_CubemapFace(unsigned int faceIndex)
{
	ASSERT(faceIndex < 6);

	return (D3DCUBEMAP_FACES)faceIndex;
}