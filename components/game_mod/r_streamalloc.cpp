#include "stdafx.h"

#if 0
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

		if (streamFrontendGlob.imageUseBits[imagePartIndex >> 5] & (1 << (imagePartIndex & 0x1F)))
		{
			__asm { lahf }
			if (!__SETP__(_AH & 0x44, 0)
				|| !(streamFrontendGlob.imageForceBits[imagePartIndex >> 5] & (1 << (imagePartIndex & 0x1F))))
			{
				if (streamFrontendGlob.imageImportance[imagePartIndex] >= importance)
					break;

				if (!(streamFrontendGlob.imageLoading[imagePartIndex >> 5] & (1 << (imagePartIndex & 0x1F))))
				{
					GfxImage *image = DB_GetImageAtIndex(imagePartIndex);

					if (image->streaming)
					{
						if (!image->skippedMipLevels)
						{
							if ((signed int)image->loadedSize >= size)
							{
								*unloadImage = image;
								return true;
							}

							if (!sacrificeImage)
								sacrificeImage = image;
						}
					}
				}
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
	streamFrontendGlob.imageUseBits[imageIndex >> 5] &= ~(1 << (imageIndex & 0x1F));

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
	for (int index = 0; index < ARRAYSIZE(streamFrontendGlob.imageUseBits); index++)
	{
		int useBits = streamFrontendGlob.imageUseBits[index];

		// If at least 1 bit is set...
		if (!useBits)
			continue;

		// Reset every index in it
		for (int bitIndex = 0; bitIndex < 32; bitIndex++)
		{
			if (useBits & (1 << (bitIndex & 0x1F)))
				streamFrontendGlob.imageUseBits[(bitIndex + 32 * index) >> 5] &= ~(1 << ((bitIndex + 32 * index) & 0x1F));
		}
	}
}
#endif