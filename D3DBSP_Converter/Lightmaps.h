#include <WTypes.h>
#include "Lump.h"

struct BO1_LMAP_PEL
{
	float B;
	float G;
	float R;
	float A;
};

struct WAW_LMAP_PEL
{
	BYTE B;
	BYTE G;
	BYTE R;
	BYTE A;

	BO1_LMAP_PEL toBO1_LMAP_PEL()
	{
		BO1_LMAP_PEL out;
		out.R = (1.0f/255.0f) * this->R;
		out.G = (1.0f/255.0f) * this->G;
		out.B = (1.0f/255.0f) * this->B;
		out.A = (1.0f/255.0f) * this->A;
		return out;
	}
};

static WAW_LMAP_PEL BO1toWAW_LMAP_PEL(BO1_LMAP_PEL boPel)
{ 
	WAW_LMAP_PEL out;
	out.R = (BYTE)(255.0 * boPel.R);
	out.G = (BYTE)(255.0 * boPel.G);
	out.B = (BYTE)(255.0 * boPel.B);
	out.A = (BYTE)(255.0 * boPel.A);
	return out;
}

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