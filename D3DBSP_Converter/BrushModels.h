#include "Lump.h"

struct DiskBrushModel_BO
{
  float mins[3];
  float maxs[3];
  unsigned __int16 firstTriSoup;
  unsigned __int16 triSoupCount;
  int firstSurface;
  int numSurfaces;
  int firstBrush;
  int numBrushes;
};

struct DiskBrushModel_WAW
{
	float mins[3];
  	float maxs[3];
	unsigned __int16 firstTriSoup;
	unsigned __int16 unk1;
	unsigned __int16 triSoupCount;
	unsigned __int16 unk2;
	int firstSurface;
  	int numSurfaces;
  	int firstBrush;
	int numBrushes;
};

int ConvertLump_WAWtoBO_BrushModels(Lump* wawLump, Lump* boLump);