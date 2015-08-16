#include "Lightmaps.h"

WAW_LMAP_PEL BO1_LMAP_PEL::toWAW_LMAP_PEL()
{
	WAW_LMAP_PEL out;
	out.R = (BYTE)(255.0 * this->R);
	out.G = (BYTE)(255.0 * this->G);
	out.B = (BYTE)(255.0 * this->B);
	out.A = (BYTE)(255.0 * this->A);
	return out;
}

BO1_LMAP_PEL WAW_LMAP_PEL::toBO1_LMAP_PEL()
{
	BO1_LMAP_PEL out;
	out.R = (1.0f/255.0f) * this->R;
	out.G = (1.0f/255.0f) * this->G;
	out.B = (1.0f/255.0f) * this->B;
	out.A = (1.0f/255.0f) * this->A;
	return out;
}

int ConvertLump_WAWtoBO_Lightmaps(Lump* wawLump, Lump* boLump)
{
	DWORD lmapCount = wawLump->size / sizeof(WAW_LMAP); //0x300000;

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
			WAWLmaps[i].small_lmap_1[p] = BO1Lmaps[i].small_lmap_1[p].toWAW_LMAP_PEL();
			WAWLmaps[i].small_lmap_2[p] = BO1Lmaps[i].small_lmap_2[p].toWAW_LMAP_PEL();
		}
		memcpy(WAWLmaps[i].big_lmap,BO1Lmaps[i].big_lmap,1024*1024);
	}

	return 0;
}