#include "BrushModels.h"

int ConvertLump_WAWtoBO_BrushModels(Lump* wawLump, Lump* boLump)
{
	DWORD brushModelCount = wawLump->size / sizeof(DiskBrushModel_WAW);
	
	DiskBrushModel_WAW* wawBrushModels = (DiskBrushModel_WAW*)wawLump->content;
	DiskBrushModel_BO* boBrushModels = (DiskBrushModel_BO*)boLump->AllocateMemory(brushModelCount * sizeof(DiskBrushModel_BO));
	
	for(DWORD i = 0; i < brushModelCount; i++)
	{
		boBrushModels[i].mins[0] = wawBrushModels[i].mins[0];
		boBrushModels[i].mins[1] = wawBrushModels[i].mins[1];
		boBrushModels[i].mins[2] = wawBrushModels[i].mins[2];

		boBrushModels[i].maxs[0] = wawBrushModels[i].maxs[0];
		boBrushModels[i].maxs[1] = wawBrushModels[i].maxs[1];
		boBrushModels[i].maxs[2] = wawBrushModels[i].maxs[2];

		boBrushModels[i].firstTriSoup = wawBrushModels[i].firstTriSoup;
		boBrushModels[i].triSoupCount = wawBrushModels[i].triSoupCount;

		boBrushModels[i].firstSurface = wawBrushModels[i].firstSurface;
		boBrushModels[i].numSurfaces = wawBrushModels[i].numSurfaces;
		boBrushModels[i].firstBrush = wawBrushModels[i].firstBrush;
		boBrushModels[i].numBrushes = wawBrushModels[i].numBrushes;
	}

	return 0;
};