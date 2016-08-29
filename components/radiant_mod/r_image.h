#pragma once

void Image_Release(GfxImage *image);
unsigned int Image_GetUsage(int imageFlags, D3DFORMAT imageFormat);
void Image_Create2DTexture_PC(GfxImage *image, unsigned __int16 width, unsigned __int16 height, int mipmapCount, int imageFlags, D3DFORMAT imageFormat);
void Image_Create3DTexture_PC(GfxImage *image, unsigned __int16 width, unsigned __int16 height, unsigned __int16 depth, int mipmapCount, int imageFlags, D3DFORMAT imageFormat);
void Image_CreateCubeTexture_PC(GfxImage *image, unsigned __int16 edgeLen, int mipmapCount, D3DFORMAT imageFormat);
void Image_PicmipForSemantic(char semantic, Picmip *picmip);
void Image_Reload(GfxImage *image);