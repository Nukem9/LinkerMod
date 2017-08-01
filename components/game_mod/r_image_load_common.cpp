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

	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A8B8G8R8:
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

// /gfx_d3d/r_image_load_common.cpp:348
int Image_SourceBytesPerSlice_PC(D3DFORMAT format, int width, int height)
{
	switch (format)
	{
	case D3DFMT_DXT1:
		return 8 * ((height + 3) >> 2) * ((width + 3) >> 2);

	case D3DFMT_DXT3:
	case D3DFMT_DXT5:
		return 16 * ((height + 3) >> 2) * ((width + 3) >> 2);

	case D3DFMT_A8R8G8B8:
	case D3DFMT_D24S8:
	case D3DFMT_R32F:
		return 4 * height * width;

	case D3DFMT_X8R8G8B8:
		return 3 * height * width;

	case D3DFMT_R5G6B5:
	case D3DFMT_A8L8:
	case D3DFMT_D16:
		return 2 * height * width;

	case D3DFMT_A8:
	case D3DFMT_L8:
		return height * width;
	}

	ASSERT_MSG_VA(false, "Unhandled case %d", format);
	return 0;
}

// /gfx_d3d/r_image_load_common.cpp:590
void Image_Upload2D_CopyDataBlock_PC(int width, int height, const char *src, D3DFORMAT format, int dstPitch, char *dst)
{
	ASSERT(src);
	ASSERT(dst);

	int srcStride = 0;
	int dy = 0;

	switch (format)
	{
	case D3DFMT_DXT1:
		srcStride = 8 * ((width + 3) >> 2);
		dy = 4;
		break;

	case D3DFMT_DXT3:
	case D3DFMT_DXT5:
		srcStride = 16 * ((width + 3) >> 2);
		dy = 4;
		break;

	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A8B8G8R8:
	case D3DFMT_G16R16:
	case D3DFMT_A16B16G16R16:
	case D3DFMT_A16B16G16R16F:
	case D3DFMT_R32F:
		srcStride = 4 * width;
		dy = 1;
		break;

	case D3DFMT_R5G6B5:
	case D3DFMT_A8L8:
		srcStride = 2 * width;
		dy = 1;
		break;

	case D3DFMT_A8:
	case D3DFMT_L8:
		srcStride = width;
		dy = 1;
		break;

	default:
		ASSERT_MSG_VA(false, "Unhandled case %d", format);
		return;
	}

	ASSERT_MSG_VA(dstPitch >= srcStride, "%i x %i: %i < %i", width, height, dstPitch, srcStride);

	if (dstPitch == srcStride)
	{
		memcpy(dst, (char *)src, srcStride * ((height - 1) / dy + 1));
	}
	else
	{
		for (int y = 0; y < height; y += dy)
		{
			memcpy(dst, (char *)src, srcStride);
			dst += dstPitch;
			src += srcStride;
		}
	}
}

// /gfx_d3d/r_image_load_common.cpp:673
void Image_Upload2D_CopyData_PC(GfxImage *image, D3DFORMAT format, D3DCUBEMAP_FACES face, unsigned int mipLevel, const char *src)
{
	D3DLOCKED_RECT lockedRect;
	memset(&lockedRect, 0, sizeof(lockedRect));

	HRESULT hr = S_OK;
	int width = max(1, image->width >> mipLevel);
	int height = max(1, image->height >> mipLevel);

	if (image->mapType == MAPTYPE_2D)
	{
		ASSERT(image->texture.map);

		hr = image->texture.map->LockRect(mipLevel, &lockedRect, nullptr, 0);

		if (FAILED(hr))
		{
			g_disableRendering++;
			Com_Error(0, "image->texture.map->LockRect(mipLevel, &lockedRect, nullptr, 0) failed: %s\n", R_ErrorDescription(hr));
		}

		Image_Upload2D_CopyDataBlock_PC(width, height, src, format, lockedRect.Pitch, (char *)lockedRect.pBits);

		hr = image->texture.map->UnlockRect(mipLevel);

		if (FAILED(hr))
		{
			g_disableRendering++;
			Com_Error(0, "image->texture.map->UnlockRect(mipLevel) failed: %s\n", R_ErrorDescription(hr));
		}
	}
	else
	{
		ASSERT(image->mapType == MAPTYPE_CUBE);
		ASSERT(
			face == D3DCUBEMAP_FACE_POSITIVE_X ||
			face == D3DCUBEMAP_FACE_NEGATIVE_X ||
			face == D3DCUBEMAP_FACE_POSITIVE_Y ||
			face == D3DCUBEMAP_FACE_NEGATIVE_Y ||
			face == D3DCUBEMAP_FACE_POSITIVE_Z ||
			face == D3DCUBEMAP_FACE_NEGATIVE_Z);
		ASSERT(image->texture.cubemap);

		hr = image->texture.cubemap->LockRect(face, mipLevel, &lockedRect, nullptr, 0);

		if (FAILED(hr))
		{
			g_disableRendering++;
			Com_Error(0, "image->texture.cubemap->LockRect(face, mipLevel, &lockedRect, nullptr, 0) failed: %s\n", R_ErrorDescription(hr));
		}

		Image_Upload2D_CopyDataBlock_PC(width, height, src, format, lockedRect.Pitch, (char *)lockedRect.pBits);

		hr = image->texture.cubemap->UnlockRect(face, mipLevel);

		if (FAILED(hr))
		{
			g_disableRendering++;
			Com_Error(0, "image->texture.cubemap->UnlockRect(face, mipLevel) failed: %s\n", R_ErrorDescription(hr));
		}
	}
}

// /gfx_d3d/r_image_load_common.cpp:824
void Image_Upload3D_CopyData_PC(GfxImage *image, D3DFORMAT format, unsigned int mipLevel, const char *src)
{
	ASSERT(image);
	ASSERT(image->mapType == MAPTYPE_3D);

	int width = max(1, image->width >> mipLevel);
	int height = max(1, image->height >> mipLevel);
	int depth = max(1, image->depth >> mipLevel);

	int srcRowPitch = Image_SourceBytesPerSlice_PC(format, width, height);

	ASSERT(image->texture.volmap);

	D3DLOCKED_BOX lockedBox;
	memset(&lockedBox, 0, sizeof(lockedBox));

	HRESULT hr = image->texture.volmap->LockBox(mipLevel, &lockedBox, nullptr, 0);

	if (FAILED(hr))
	{
		g_disableRendering++;
		Com_Error(0, "image->texture.volmap->LockBox(mipLevel, &lockedBox, nullptr, 0) failed: %s\n", R_ErrorDescription(hr));
	}

	char *dst = (char *)lockedBox.pBits;
	for (int sliceIndex = 0; sliceIndex < depth; sliceIndex++)
	{
		Image_Upload2D_CopyDataBlock_PC(width, height, src, format, lockedBox.RowPitch, dst);
		src += srcRowPitch;
		dst += lockedBox.SlicePitch;
	}

	hr = image->texture.volmap->UnlockBox(mipLevel);

	if (FAILED(hr))
	{
		g_disableRendering++;
		Com_Error(0, "image->texture.volmap->UnlockBox(mipLevel) failed: %s\n", R_ErrorDescription(hr));
	}

	image->texture.volmap->PreLoad();
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
void Image_Setup(GfxImage *image, int width, int height, int depth, int imageFlags, D3DFORMAT imageFormat, const char *name, unsigned int allocFlags)
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