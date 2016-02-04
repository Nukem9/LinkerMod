#include <WTypes.h>
#include "Lump.h"

struct WAW_LMAP_PEL;

struct BO1_LMAP_PEL
{
	float B;
	float G;
	float R;
	float A;

	operator WAW_LMAP_PEL() const;
};

struct WAW_LMAP_PEL
{
	BYTE B;
	BYTE G;
	BYTE R;
	BYTE A;

	operator BO1_LMAP_PEL() const;
};

struct BO1_LMAP
{
	BO1_LMAP_PEL small_lmap_1[512*512];
	BO1_LMAP_PEL small_lmap_2[512*512];
	BYTE big_lmap[1024*1024];
};

struct WAW_LMAP
{
	WAW_LMAP_PEL small_lmap_1[512*512];
	WAW_LMAP_PEL small_lmap_2[512*512];
	BYTE big_lmap[1024*1024];
};

int ConvertLump_WAWtoBO_Lightmaps(Lump* wawLump, Lump* boLump);
int ConvertLump_BOtoWAW_Lightmaps(Lump* boLump, Lump* wawLump);