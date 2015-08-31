#include "stdafx.h"

SRCLINE(309)
void Image_Release(GfxImage *image)
{
	((void(__cdecl *)(GfxImage *))0x0052ABC0)(image);
}

SRCLINE(792)
void Image_PicmipForSemantic(char semantic, Picmip *picmip)
{
	signed int picmipUsed = 0;

	switch (semantic)
	{
	default:
		ASSERT(false && "Unhandled case");

	case 0:
	case 1:
		picmip->platform[0] = 0;
		picmip->platform[1] = 0;
		return;

	case 2:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
		picmipUsed = *(DWORD *)0x1567F28;
		break;

	case 5:
		picmipUsed = *(DWORD *)0x1567F2C;
		break;

	case 8:
		picmipUsed = *(DWORD *)0x1567F30;
		break;
	}

	picmip->platform[1] = 2;

	if (picmipUsed >= 0)
	{
		if (picmipUsed > 3)
			picmipUsed = 3;
	}
	else
	{
		picmipUsed = 0;
	}

	picmip->platform[0] = picmipUsed;
}

SRCLINE(1401)
void Image_Reload(GfxImage *image)
{
	ASSERT(image != nullptr);

	Image_Release(image);

	if (!Image_LoadFromFile(image))
		Com_PrintError(8, "failed to load image '%s'\n", image->name);
}