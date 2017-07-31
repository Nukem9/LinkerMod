#include "stdafx.h"

char *s_imageLoadBuf;
unsigned int s_imageLoadBytesUsed;

// /gfx_d3d/r_image_load_obj.cpp:103
char Image_GetPcStreamedMips(GfxImageFileHeader *fileHeader)
{
	char streamedMipLevels = 0;

	if (!(fileHeader->flags & 16))
		return 0;

	if (fileHeader->flags & 12)
		return 0;

	int minDimension = min(fileHeader->dimensions[1], fileHeader->dimensions[0]);

	for (; minDimension > 128; minDimension >>= 1)
		streamedMipLevels++;

	return streamedMipLevels;
}

// /gfx_d3d/r_image_load_obj.cpp:127
bool Image_ValidateHeader(GfxImageFileHeader *imageFile, const char *filepath)
{
	// First 3 bytes of the file is always 'IWi'
	if (imageFile->tag[0] != 'I' || imageFile->tag[1] != 'W' || imageFile->tag[2] != 'i')
	{
		Com_PrintError(8, "ERROR: image '%s' is not an IW image\n", filepath);
		return false;
	}

	if (imageFile->version != 13)
	{
		Com_PrintError(8, "ERROR: image '%s' is version %i but should be version %i\n", filepath, imageFile->version, 13);
		return false;
	}

	// Passed validation
	return true;
}

// /gfx_d3d/r_image_load_obj.cpp:187
void Image_SetupFromFile(GfxImage *image, GfxImageFileHeader *fileHeader, D3DFORMAT imageFormat, unsigned int allocFlags)
{
	ASSERT(image != nullptr);
	ASSERT(fileHeader != nullptr);

	char picmip = image->picmip.platform[useFastFile->current.enabled == 0];
	int width = max(1, fileHeader->dimensions[0] >> picmip);
	int height = max(1, fileHeader->dimensions[1] >> picmip);
	int depth = max(1, fileHeader->dimensions[2] >> picmip);

	Image_Setup(image, width, height, depth, fileHeader->flags, imageFormat, "<file>", allocFlags);

	ASSERT(image->cardMemory.platform[PICMIP_PLATFORM_USED] > 0);
}

// /gfx_d3d/r_image_load_obj.cpp:210
int Image_CountMipmapsForFile(GfxImageFileHeader *fileHeader)
{
	return Image_CountMipmaps(
		fileHeader->flags,
		fileHeader->dimensions[0],
		fileHeader->dimensions[1],
		fileHeader->dimensions[2]);
}

// /gfx_d3d/r_image_load_obj.cpp:221
void Image_ExpandBgr(const char *src, unsigned int count, char *dst)
{
	ASSERT(src);
	ASSERT(dst);
	ASSERT(count > 0);

	do
	{
		dst[0] = src[0];
		dst[1] = src[1];
		dst[2] = src[2];
		dst[3] = (char)0xFF;

		dst += 4;
		src += 3;

		--count;
	} while (count);
}

// /gfx_d3d/r_image_load_obj.cpp:257
char *Image_AllocTempMemory(int bytes)
{
	// Align to nearest 4-byte boundary
	bytes = (bytes + 3) & 0xFFFFFFFC;

	if ((bytes + s_imageLoadBytesUsed) > 0x1000000)
		Com_Error(ERR_DROP, "Needed to allocate at least %.1f MB to load images", (float)(bytes + s_imageLoadBytesUsed) / (1.0f * 1024.0f * 1024.0f));

	if (!s_imageLoadBuf)
	{
		s_imageLoadBuf = (char *)Z_VirtualAlloc(0x1000000, "Image_AllocTempMemory", 19);
		ASSERT(s_imageLoadBuf);
	}

	char *mem = &s_imageLoadBuf[s_imageLoadBytesUsed];
	s_imageLoadBytesUsed += bytes;
	return mem;
}

// /gfx_d3d/r_image_load_obj.cpp:280
void Image_FreeTempMemory(char *mem, int bytes)
{
	// Align to nearest 4-byte boundary
	bytes = (bytes + 3) & 0xFFFFFFFC;

	ASSERT((mem + bytes) == (s_imageLoadBuf + s_imageLoadBytesUsed));

	s_imageLoadBytesUsed -= bytes;
}

// /gfx_d3d/r_image_load_obj.cpp:293
void Image_LoadWavelet(GfxImage *image, GfxImageFileHeader *fileHeader, const char *data, D3DFORMAT format, int bytesPerPixel, unsigned int allocFlags)
{
	ASSERT(image);
	ASSERT(fileHeader);
	ASSERT(data);

	static DWORD dwCall = 0x00736170;

	__asm
	{
		/* allocFlags unused */
		mov eax, bytesPerPixel
		mov ecx, fileHeader
		push format
		push data
		push image
		call [dwCall]
		add esp, 0xC
	}
}

// /gfx_d3d/r_image_load_obj.cpp:365
void Image_LoadBitmap(GfxImage *image, GfxImageFileHeader *fileHeader, char *data, D3DFORMAT format, int bytesPerPixel, int allocFlags)
{
	ASSERT(image != nullptr);
	ASSERT(fileHeader != nullptr);
	ASSERT(data != nullptr);

	Image_SetupFromFile(image, fileHeader, format, allocFlags);

	// 4 bytes per pixel in bitmaps
	char *expandedData = nullptr;
	int expandedSize = 4 * image->height * image->width * image->depth;

	if (format == D3DFMT_X8R8G8B8)
		expandedData = Image_AllocTempMemory(expandedSize);

	int faceCount = (image->mapType == MAPTYPE_CUBE) ? 6 : 1;
	int mipcount = Image_CountMipmapsForFile(fileHeader);
	int picmip = image->picmip.platform[useFastFile->current.enabled == false];

	for (int mipLevel = mipcount - 1; mipLevel >= picmip; mipLevel--)
	{
		int width = max(1, fileHeader->dimensions[0] >> picmip);
		int height = max(1, fileHeader->dimensions[1] >> picmip);
		int depth = max(1, fileHeader->dimensions[2] >> picmip);

		for (int faceIndex = 0; faceIndex < faceCount; faceIndex++)
		{
			D3DCUBEMAP_FACES face = Image_CubemapFace(faceIndex);

			if (format == D3DFMT_X8R8G8B8)
			{
				Image_ExpandBgr(data, depth * height * width, expandedData);
				Image_UploadData(image, D3DFMT_X8R8G8B8, face, mipLevel - picmip, expandedData);
			}
			else
			{
#if 0
				if (format == D3DFMT_A8R8G8B8)
					Image_InPlaceSwizzleBgra(data);
#endif

				Image_UploadData(image, format, face, mipLevel - picmip, data);
			}

			data += bytesPerPixel * height * width * depth;
		}
	}

	if (expandedData)
		Image_FreeTempMemory(expandedData, expandedSize);
}

// /gfx_d3d/r_image_load_obj.cpp:429
void Image_LoadDxtc(GfxImage *image, GfxImageFileHeader *fileHeader, const char *data, D3DFORMAT format, int bytesPerBlock, unsigned int allocFlags)
{
	ASSERT(image);
	ASSERT(fileHeader);
	ASSERT(data);
	ASSERT(format == D3DFMT_A16B16G16R16F || format == D3DFMT_DXT1 || format == D3DFMT_DXT3 || format == D3DFMT_DXT5);

	Image_SetupFromFile(image, fileHeader, format, allocFlags);

	int faceCount = (image->mapType == MAPTYPE_CUBE) ? 6 : 1;
	int mipcount = Image_CountMipmapsForFile(fileHeader);
	int picmip = image->picmip.platform[useFastFile->current.enabled == false];

	for (int mipLevel = mipcount - 1; mipLevel >= picmip; mipLevel--)
	{
		int width = max(1, fileHeader->dimensions[0] >> picmip);
		int height = max(1, fileHeader->dimensions[1] >> picmip);

		for (int faceIndex = 0; faceIndex < faceCount; faceIndex++)
		{
			D3DCUBEMAP_FACES face = Image_CubemapFace(faceIndex);

			Image_UploadData(image, format, face, mipLevel - picmip, data);
			data += bytesPerBlock * ((width + 3) >> 2) * ((height + 3) >> 2);
		}
	}
}

// /gfx_d3d/r_image_load_obj.cpp:536
void Image_LoadFromData(GfxImage *image, GfxImageFileHeader *fileHeader, char *srcData, unsigned int allocFlags)
{
	static DWORD dwCall = 0x007366C0;

	__asm
	{
		mov edx, srcData
		mov ecx, fileHeader
		push image
		call [dwCall]
		add esp, 0x4
	}

	return;

	image->loadedSize		= fileHeader->fileSizeForPicmip[image->skippedMipLevels] - sizeof(GfxImageFileHeader);
	image->baseSize			= fileHeader->fileSizeForPicmip[0] - sizeof(GfxImageFileHeader);
	image->texture.basemap	= nullptr;
	Image_TrackBytes		+= image->loadedSize;

	switch (fileHeader->format)
	{
	case IMG_FORMAT_BITMAP_RGBA:			Image_LoadBitmap(image, fileHeader, srcData, D3DFMT_A8R8G8B8, 4, allocFlags); break;
	case IMG_FORMAT_BITMAP_RGB:				Image_LoadBitmap(image, fileHeader, srcData, D3DFMT_X8R8G8B8, 3, allocFlags); break;
	case IMG_FORMAT_BITMAP_LUMINANCE_ALPHA:	Image_LoadBitmap(image, fileHeader, srcData, D3DFMT_A8L8, 2, allocFlags); break;
	case IMG_FORMAT_BITMAP_LUMINANCE:		Image_LoadBitmap(image, fileHeader, srcData, D3DFMT_L8, 1, allocFlags); break;
	case IMG_FORMAT_BITMAP_ALPHA:			Image_LoadBitmap(image, fileHeader, srcData, D3DFMT_A8, 1, allocFlags); break;
	case IMG_FORMAT_WAVELET_RGBA:			Image_LoadWavelet(image, fileHeader, srcData, D3DFMT_A8R8G8B8, 4, allocFlags); break;
	case IMG_FORMAT_WAVELET_RGB:			Image_LoadWavelet(image, fileHeader, srcData, D3DFMT_X8R8G8B8, 3, allocFlags); break;
	case IMG_FORMAT_WAVELET_LUMINANCE_ALPHA:Image_LoadWavelet(image, fileHeader, srcData, D3DFMT_A8L8, 2, allocFlags); break;
	case IMG_FORMAT_WAVELET_LUMINANCE:		Image_LoadWavelet(image, fileHeader, srcData, D3DFMT_L8, 1, allocFlags); break;
	case IMG_FORMAT_WAVELET_ALPHA:			Image_LoadWavelet(image, fileHeader, srcData, D3DFMT_A8, 1, allocFlags); break;
	case IMG_FORMAT_DXT1:					Image_LoadDxtc(image, fileHeader, srcData, D3DFMT_DXT1, 8, allocFlags); break;
	case IMG_FORMAT_DXT3:					Image_LoadDxtc(image, fileHeader, srcData, D3DFMT_DXT3, 16, allocFlags); break;
	case IMG_FORMAT_DXT5:					Image_LoadDxtc(image, fileHeader, srcData, D3DFMT_DXT5, 16, allocFlags); break;
	case IMG_FORMAT_A16B16G16R16F:			Image_LoadDxtc(image, fileHeader, srcData, D3DFMT_A16B16G16R16F, 128, allocFlags); break;

	default:
		ASSERT_MSG_VA(false, "Unhandled case %i", fileHeader->format);
		break;
	}
}

// /gfx_d3d/r_image_load_obj.cpp:627
void Image_PrintTruncatedFileError(const char *filepath)
{
	Com_PrintError(8, "ERROR: image '%s' is truncated. Delete the file and run converter to fix.\n", filepath);
}

// /gfx_d3d/r_image_load_obj.cpp:1303
bool Image_LoadToBuffer(GfxImage *image, bool loadHighmip, char **imageBuffer, int *bufferSize)
{
	ASSERT(image);
	ASSERT(image->category == IMG_CATEGORY_LOAD_FROM_FILE);

	*imageBuffer = nullptr;
	*bufferSize = 0;

	// Build image local path
	char filepath[64];
	if (sprintf_s(filepath, "images/%s.iwi", image->name) < 0)
	{
		Com_PrintError(8, "ERROR: Filename '%s' too long\n", filepath);
		return false;
	}

	// Check if the file exists
	int fileHandle;
	int fileSize = FS_FOpenFileRead(filepath, &fileHandle);

	if (fileSize < 0)
	{
		Com_PrintError(8, "ERROR: Image '%s' is missing\n", filepath);
		return false;
	}

	// Read the initial header and validate
	GfxImageFileHeader fileHeader;

	ASSERT(fileSize >= sizeof(fileHeader));

	if (FS_Read(&fileHeader, sizeof(fileHeader), fileHandle) != sizeof(fileHeader))
	{
		Image_PrintTruncatedFileError(filepath);
		FS_FCloseFile(fileHandle);
		return false;
	}

	if (!Image_ValidateHeader(&fileHeader, filepath))
	{
		FS_FCloseFile(fileHandle);
		return false;
	}

	// Determine if mipmapping was explicitly disabled on image
	if ((fileHeader.flags & 3) || min(fileHeader.dimensions[1], fileHeader.dimensions[0]) < 32)
		image->noPicmip = true;

	Image_GetPicmip(image, &image->picmip);

	if (fileHeader.fileSizeForPicmip[0] != fileSize)
	{
		Com_PrintError(
			8,
			"ERROR: Image '%s' is invalid. fileHeader.fileSizeForPicmip[0] != fileSize (%d != %d)\n",
			filepath,
			fileHeader.fileSizeForPicmip[0],
			fileSize);
		FS_FCloseFile(fileHandle);
		return false;
	}

	int picmip = image->picmip.platform[useFastFile->current.enabled == false];
	char streamedMipLevels = picmip > 0;

	ASSERT_MSG(streamedMipLevels >= 0 && streamedMipLevels < PICMIP_SIZES_STORED, "streamedMipLevels doesn't index PICMIP_SIZES_STORED");

	// Read the raw data from disk
	int readSize = fileHeader.fileSizeForPicmip[streamedMipLevels];

	*imageBuffer = (char *)Z_TryVirtualAlloc(readSize, "Image_AllocTempMemory", 20);
	*bufferSize = readSize;

	if (*imageBuffer == nullptr)
	{
		Com_PrintError(8, "ERROR: Failed to allocate memory for image '%s' (%d bytes)\n", filepath, readSize);
		FS_FCloseFile(fileHandle);
		return false;
	}

	// Read header and adjust remainder accordingly
	memcpy(*imageBuffer, &fileHeader, sizeof(fileHeader));
	readSize -= sizeof(fileHeader);

	if (FS_Read(*imageBuffer + sizeof(fileHeader), readSize, fileHandle) != readSize)
	{
		Image_PrintTruncatedFileError(filepath);
		Z_VirtualFree(*imageBuffer, 20);
		FS_FCloseFile(fileHandle);
	}

	streamedMipLevels = Image_GetPcStreamedMips(&fileHeader);
	image->streaming = streamedMipLevels > 0;

	if (!image->streaming || loadHighmip)
	{
		image->skippedMipLevels = 0;
		streamedMipLevels = 0;
	}
	else
	{
		fileHeader.dimensions[0] >>= streamedMipLevels;
		fileHeader.dimensions[1] >>= streamedMipLevels;
		image->skippedMipLevels = streamedMipLevels;
	}

	FS_FCloseFile(fileHandle);
	return true;
}