#include "LightGridColors.h"
#include "ConsoleLog.h"

int ConvertLump_WAWtoBO_LightGridColors(Lump* wawLump, Lump* boLump)
{
	DWORD LightGridColorCount = wawLump->size / sizeof(DiskGfxLightGridColors_WAW);

	DiskGfxLightGridColors_WAW* WAWLightGridColors = (DiskGfxLightGridColors_WAW*)wawLump->content;
	DiskGfxLightGridColors_BO* BOLightGridColors = (DiskGfxLightGridColors_BO*)boLump->AllocateMemory(LightGridColorCount * sizeof(DiskGfxLightGridColors_BO));

	memset(BOLightGridColors, 0, LightGridColorCount * sizeof(DiskGfxLightGridColors_BO));

	BYTE* src = (BYTE*)WAWLightGridColors;
	WORD* dst = (WORD*)BOLightGridColors;

	for(DWORD i = 0; i < LightGridColorCount * 56 * 3; i++)
	{
		dst[i] = src[i];
	}

	return 0;
}

int ConvertLump_BOtoWAW_LightGridColors(Lump* boLump, Lump* wawLump)
{
	DWORD LightGridColorCount = boLump->size / sizeof(DiskGfxLightGridColors_BO);

	DiskGfxLightGridColors_BO* BOLightGridColors = (DiskGfxLightGridColors_BO*)boLump->content;
	DiskGfxLightGridColors_WAW* WAWLightGridColors = (DiskGfxLightGridColors_WAW*)wawLump->AllocateMemory(LightGridColorCount * sizeof(DiskGfxLightGridColors_WAW));

	memset(WAWLightGridColors, 0, LightGridColorCount * sizeof(DiskGfxLightGridColors_WAW));

	WORD* src = (WORD*)BOLightGridColors;
	BYTE* dst = (BYTE*)WAWLightGridColors;

	for(DWORD i = 0; i < LightGridColorCount * 56 * 3; i++)
	{
		dst[i] = (BYTE)src[i];
	}

	return 0;
}