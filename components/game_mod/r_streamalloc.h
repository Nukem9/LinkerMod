#pragma once

#if 0
struct StreamTempImage
{
	GfxImage *image;
	bool alloced;
};

struct StreamAllocGlob
{
	FastCriticalSection lock;
	StreamTempImage tempImages[32];
	bool tempImagesInit;
};

void R_StreamAlloc_InitTempImages();
void R_StreamAlloc_Lock();
void R_StreamAlloc_Unlock();
bool R_StreamAlloc_FreeImageByImportance(int size, float importance, GfxImage **unloadImage);
bool R_StreamAlloc_CanAllocate(int size, float importance, GfxImage **unloadImage);
void R_StreamAlloc_Flush();
#endif