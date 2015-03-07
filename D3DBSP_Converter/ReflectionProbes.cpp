#include "ReflectionProbes.h"

#include <fstream>
int ConvertLump_WAWtoBO_ReflectionProbes(Lump* wawLump, Lump* boLump)
{
	DWORD probeCount = (wawLump->size / sizeof(DiskGfxReflectionProbe_WAW));
	
	DiskGfxReflectionProbe_WAW* WAWReflectionProbes = (DiskGfxReflectionProbe_WAW*)wawLump->content;
	DiskGfxReflectionProbe_BO* BOReflectionProbes = (DiskGfxReflectionProbe_BO*)boLump->AllocateMemory(probeCount * sizeof(DiskGfxReflectionProbe_BO));
	
	for(DWORD i = 0; i < probeCount; i++)
	{
		memset(&BOReflectionProbes[i],0,sizeof(DiskGfxReflectionProbe_BO));

		strcpy_s(BOReflectionProbes[i].colorCorrectionFilename,WAWReflectionProbes[i].colorCorrectionFilename);

		//Currently sets all reflection images to black
		for(DWORD p = 0; p < 262224; p++)
		{
			if(p%4 < p%8)
			{
				BOReflectionProbes[i].pixels[p] = 0xAA;
			}
			else
			{
				BOReflectionProbes[i].pixels[p] = 0x00;
			}
		}

		BOReflectionProbes[i].origin[0] = WAWReflectionProbes[i].origin[0];
		BOReflectionProbes[i].origin[1] = WAWReflectionProbes[i].origin[1];
		BOReflectionProbes[i].origin[2] = WAWReflectionProbes[i].origin[2];
	}

	return 0;
}