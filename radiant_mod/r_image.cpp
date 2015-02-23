#include "stdafx.h"

SRCLINE(309)
void Image_Release(GfxImage *image)
{
	((void(__cdecl *)(GfxImage *))0x0052ABC0)(image);
}

SRCLINE(1401)
void Image_Reload(GfxImage *image)
{
	ASSERT(image != nullptr);

	Image_Release(image);

	if (!Image_LoadFromFile(image))
		Com_PrintError(8, "failed to load image '%s'\n", image->name);
}