#include "stdafx.h"

StreamAllocGlob s_allocGlob;

// /gfx_d3d/r_streamalloc.cpp:1716
void R_StreamAlloc_InitTempImages()
{
	if (s_allocGlob.tempImagesInit)
		return;

	for (int index = 0; index < ARRAYSIZE(s_allocGlob.tempImages); index++)
	{
		s_allocGlob.tempImages[index].image = DB_AllocTempImage();
		s_allocGlob.tempImages[index].alloced = false;

		memset(s_allocGlob.tempImages[index].image, 0, sizeof(GfxImage));
	}

	s_allocGlob.tempImagesInit = true;
}

// /gfx_d3d/r_streamalloc.cpp:1751
GfxImage *R_StreamAlloc_SetupTempImage(D3DFORMAT format, bool linear, int width, int height, int mipLevels)
{
	ASSERT(s_allocGlob.tempImagesInit);

	R_StreamAlloc_Lock();

	// Try to find a free image slot
	GfxImage *image = nullptr;

	for (int index = 0; index < ARRAYSIZE(s_allocGlob.tempImages); index++)
	{
		if (s_allocGlob.tempImages[index].alloced)
			continue;

		s_allocGlob.tempImages[index].alloced = true;
		image = s_allocGlob.tempImages[index].image;
		break;
	}

	// We found one, so register it with D3D
	if (image)
	{
		Image_Create2DTexture_PC(image, width, height, 0, 0, format);
		R_StreamAlloc_Unlock();
		return image;
	}

	R_StreamAlloc_Unlock();
	return nullptr;
}

// /gfx_d3d/r_streamalloc.cpp:1784
void R_StreamAlloc_ReleaseTempImage(GfxImage *image)
{
	R_StreamAlloc_Lock();

	for (int index = 0; index < ARRAYSIZE(s_allocGlob.tempImages); index++)
	{
		if (image != s_allocGlob.tempImages[index].image)
			continue;

		Image_Release(image);
		s_allocGlob.tempImages[index].alloced = false;

		R_StreamAlloc_Unlock();
		return;
	}

	ASSERT_MSG(false, "Tried to release a non-temp image");
	R_StreamAlloc_Unlock();
}

// /gfx_d3d/r_streamalloc.cpp:1804
void R_StreamAlloc_Lock()
{
	Sys_LockWrite(&s_allocGlob.lock);
}

// /gfx_d3d/r_streamalloc.cpp:1809
void R_StreamAlloc_Unlock()
{
	Sys_UnlockWrite(&s_allocGlob.lock);
}

// /gfx_d3d/r_streamalloc.cpp:1815
bool R_StreamAlloc_FreeImageByImportance(int size, float importance, GfxImage **unloadImage)
{
	GfxImage *sacrificeImage = nullptr;

	for (int sortIndex = streamFrontendGlob.sortedImageCount - 1; sortIndex >= 0; sortIndex--)
	{
		int imagePartIndex = streamFrontendGlob.sortedImages[sortIndex];

		const unsigned int imageIndex = BIT_INDEX_32(imagePartIndex);
		const unsigned int imageMask = BIT_MASK_32(imagePartIndex);

		if (!(streamFrontendGlob.imageUseBits[imageIndex] & imageMask))
			continue;

		// FLT_MAX acts as a wildcard
		if (importance == FLT_MAX || !(streamFrontendGlob.imageForceBits[imageIndex] & imageMask))
		{
			// Kill the search if we hit a higher priority image
			if (streamFrontendGlob.imageImportance[imagePartIndex] >= importance)
				break;

			if (streamFrontendGlob.imageLoading[imageIndex] & imageMask)
				continue;

			GfxImage *image = DB_GetImageAtIndex(imagePartIndex);

			if (image->streaming == GFX_NOT_STREAMING)
				continue;

			if (!image->skippedMipLevels)
			{
				if (image->loadedSize >= (unsigned int)size)
				{
					*unloadImage = image;
					return true;
				}

				if (!sacrificeImage)
					sacrificeImage = image;
			}
		}
	}

	if (sacrificeImage)
	{
		*unloadImage = sacrificeImage;
		return true;
	}

	*unloadImage = nullptr;
	return false;
}

// /gfx_d3d/r_streamalloc.cpp:1894
bool R_StreamAlloc_CanAllocate(int size, float importance, GfxImage **unloadImage)
{
	*unloadImage = nullptr;

	if ((Image_TrackBytes + size) <= streamFrontendGlob.mainBufferSize)
		return true;

	return R_StreamAlloc_FreeImageByImportance(size, importance, unloadImage);
}

// /gfx_d3d/r_streamalloc.cpp:1904
char *R_StreamAlloc_FreeImage(GfxImage *image, int imagePart, bool dirty, int *freedSize)
{
	int imageIndex = DB_GetImageIndex(image);
	streamFrontendGlob.imageUseBits[BIT_INDEX_32(imageIndex)] &= ~BIT_MASK_32(imageIndex);

	*freedSize = 0;
	return nullptr;
}

// /gfx_d3d/r_streamalloc.cpp:???
void R_StreamAlloc_Deallocate(char *imageMemory)
{
	// Code removed completely
}

// /gfx_d3d/r_streamalloc.cpp:1917
void R_StreamAlloc_Flush()
{
	for (int index = 0; index < STREAM_MAX_IMAGE_BITS; index++)
	{
		int useBits = streamFrontendGlob.imageUseBits[index];

		// If at least 1 bit is set...
		if (!useBits)
			continue;

		// Reset every index in it
		for (int bitIndex = 0; bitIndex < 32; bitIndex++)
		{
			if (useBits & BIT_MASK_32(bitIndex))
				streamFrontendGlob.imageUseBits[BIT_INDEX_32(bitIndex + 32 * index)] &= ~BIT_MASK_32(bitIndex + 32 * index);
		}
	}
}