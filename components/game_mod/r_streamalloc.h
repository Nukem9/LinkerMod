#pragma once

struct FastCriticalSection
{

};

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
char *R_StreamAlloc_FreeImage(GfxImage *image, int imagePart, bool dirty, int *freedSize);
void R_StreamAlloc_Deallocate(char *imageMemory);
void R_StreamAlloc_Flush();