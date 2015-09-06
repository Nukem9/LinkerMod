#pragma once

char Image_GetPcStreamedMips(GfxImageFileHeader *fileHeader);
bool Image_ValidateHeader(GfxImageFileHeader *imageFile, const char *filepath);
void Image_SetupFromFile(GfxImage *image, GfxImageFileHeader *fileHeader, D3DFORMAT imageFormat, unsigned int allocFlags);
void Image_ExpandBgr(const char *src, unsigned int count, char *dst);
void Image_LoadBitmap(GfxImage *image, GfxImageFileHeader *fileHeader, char *data, D3DFORMAT format, int bytesPerPixel, int allocFlags);
void Image_LoadDxtc(GfxImage *image, GfxImageFileHeader *fileHeader, const char *data, D3DFORMAT format, int bytesPerBlock, unsigned int allocFlags);
void Image_LoadFromData(GfxImage *image, GfxImageFileHeader *fileHeader, char *srcData, unsigned int allocFlags);
void Image_PrintTruncatedFileError(const char *filepath);
void Image_UploadData(GfxImage *image, D3DFORMAT format, D3DCUBEMAP_FACES face, unsigned int mipLevel, const char *src);
bool Image_LoadFromFileWithReader(GfxImage *image, int(__cdecl * OpenFileRead)(const char *, int *));
bool Image_LoadFromFile(GfxImage *image);
int Image_CountMipmapsForFile(GfxImageFileHeader *fileHeader);

void hk_Image_LoadFromFileWithReader();