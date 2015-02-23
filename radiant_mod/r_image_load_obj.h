#pragma once

#define PICMIP_PLATFORM_USED 0

bool Image_ValidateHeader(GfxImageFileHeader *imageFile, const char *filepath);
bool Image_LoadFromFile(GfxImage *image);
bool Image_LoadFromFileWithReader(GfxImage *image, int(__cdecl * OpenFileRead)(const char *, int *));
void Image_PrintTruncatedFileError(const char *filepath);
void Image_LoadFromData(GfxImage *image, GfxImageFileHeader *fileHeader, char *srcData, unsigned int allocFlags);
void Image_UploadData(GfxImage *image, _D3DFORMAT format, D3DCUBEMAP_FACES face, unsigned int mipLevel, const char *src);
void Image_LoadBitmap(GfxImage *image, GfxImageFileHeader *fileHeader, char *data, D3DFORMAT format, int bytesPerPixel, int allocFlags);
void Image_LoadDxtc(GfxImage *image, GfxImageFileHeader *fileHeader, const char *data, D3DFORMAT format, int bytesPerBlock, unsigned int allocFlags);
void Image_ExpandBgr(const char *src, unsigned int count, char *dst);
void Image_SetupFromFile(GfxImage *image, GfxImageFileHeader *fileHeader, D3DFORMAT imageFormat, unsigned int allocFlags);
int Image_CountMipmapsForFile(GfxImageFileHeader *fileHeader);
unsigned int Image_CountMipmaps(unsigned int imageFlags, unsigned int width, unsigned int height, unsigned int depth);
char Image_GetPcStreamedMips(GfxImageFileHeader *fileHeader);

void hk_Image_LoadFromFileWithReader();