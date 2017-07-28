#pragma once

void Image_TrackTexture(GfxImage *image, int imageFlags, D3DFORMAT format, int width, int height, int depth);
unsigned int Image_GetCardMemoryAmountForMipLevel(D3DFORMAT format, unsigned int mipWidth, unsigned int mipHeight, unsigned int mipDepth);
unsigned int Image_GetCardMemoryAmount(unsigned int imageFlags, D3DFORMAT format, unsigned int width, unsigned int height, unsigned int depth);
void Image_Upload2D_CopyData_PC(GfxImage *image, D3DFORMAT format, D3DCUBEMAP_FACES face, unsigned int mipLevel, const char *src);
void Image_Upload3D_CopyData_PC(GfxImage *image, D3DFORMAT format, unsigned int mipLevel, const char *src);
void Image_UploadData(GfxImage *image, D3DFORMAT format, D3DCUBEMAP_FACES face, unsigned int mipLevel, const char *src);;
unsigned int Image_CountMipmaps(unsigned int imageFlags, unsigned int width, unsigned int height, unsigned int depth);
void Image_GetPicmip(GfxImage *image, Picmip *picmip);
void Image_Setup(GfxImage *image, int width, int height, int depth, char imageFlags, D3DFORMAT imageFormat, const char *name, unsigned int allocFlags);
D3DCUBEMAP_FACES Image_CubemapFace(unsigned int faceIndex);