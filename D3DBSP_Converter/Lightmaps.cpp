#include "Lightmaps.h"

int ConvertLump_WAWtoBO_Lightmaps(Lump* wawLump, Lump* boLump)
{
	DWORD lmapCount = wawLump->size / sizeof(WAW_LMAP);/// 0x300000;

	WAW_LMAP* WAWLmaps = (WAW_LMAP*)wawLump->content;
	BO1_LMAP* BO1Lmaps = (BO1_LMAP*)boLump->AllocateMemory(sizeof(BO1_LMAP) * lmapCount);//0x00900000

	for(DWORD i = 0; i < lmapCount; i++)
	{
		for(DWORD p = 0; p < 512*512; p++)
		{
			BO1Lmaps[i].small_lmap_1[p] = WAWLmaps[i].small_lmap_1[p].toBO1_LMAP_PEL();
			BO1Lmaps[i].small_lmap_2[p] = WAWLmaps[i].small_lmap_2[p].toBO1_LMAP_PEL();
		}
		memcpy(BO1Lmaps[i].big_lmap,WAWLmaps[i].big_lmap,1024*1024);
	}

	return 0;
}

int ConvertLump_BOtoWAW_Lightmaps(Lump* boLump, Lump* wawLump)
{
	DWORD lmapCount = boLump->size / sizeof(BO1_LMAP); //0x00900000

	BO1_LMAP* BO1Lmaps = (BO1_LMAP*)boLump->content;
	WAW_LMAP* WAWLmaps = (WAW_LMAP*)wawLump->AllocateMemory(sizeof(WAW_LMAP) * lmapCount); //0x300000;

	for(DWORD i = 0; i < lmapCount; i++)
	{
		for(DWORD p = 0; p < 512*512; p++)
		{
			WAWLmaps[i].small_lmap_1[p] = BO1toWAW_LMAP_PEL(BO1Lmaps[i].small_lmap_1[p]);
			WAWLmaps[i].small_lmap_2[p] = BO1toWAW_LMAP_PEL(BO1Lmaps[i].small_lmap_2[p]);
		}
		memcpy(WAWLmaps[i].big_lmap,BO1Lmaps[i].big_lmap,1024*1024);
	}

	return 0;
}