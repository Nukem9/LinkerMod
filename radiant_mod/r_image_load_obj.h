#pragma once

bool Image_ValidateHeader(GfxImageFileHeader *imageFile, const char *filepath);
bool Image_LoadFromFileWithReader(GfxImage *image, int(__cdecl * OpenFileRead)(const char *, int *));

void hk_Image_LoadFromFileWithReader();