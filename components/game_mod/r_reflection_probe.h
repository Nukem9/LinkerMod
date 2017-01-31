#pragma once
#include "r_scene.h"

#define MAX_MAP_REFLECTION_PROBES 64

typedef int CubemapShot;

struct DiskGfxCubemapFace256
{
	char mip0[32768]; //256x256
	char mip1[8192]; //128x128
	char mip2[2048]; //64x64
	char mip3[512]; //32x32
	char mip4[128]; //16x16
	char mip5[32]; //8x8
	char mip6[8]; //4x4
	char mip7[8]; //2x2
	char mip8[8]; //1x1
};

struct DiskGfxReflectionProbeVolumeData
{
	float volumePlanes[6][4];
};

struct DiskGfxReflectionProbe
{
	float origin[3];

	union
	{
		char pixels[262224];
		DiskGfxCubemapFace256 face;
	};

	DiskGfxReflectionProbeVolumeData probeVolumes[16];
	unsigned int probeVolumeCount;
	char colorCorrectionFilename[64];
	char name[64];
};
static_assert(sizeof(DiskGfxReflectionProbe) == 0x406E0, "Size mismatch");

struct GfxReflectionProbeVolumeData
{
	float volumePlanes[6][4];
};

struct GfxReflectionProbe
{
	float origin[3];
	GfxImage *reflectionImage;
	GfxReflectionProbeVolumeData *probeVolumes;
	unsigned int probeVolumeCount;
};
static_assert(sizeof(GfxReflectionProbe) == 0x18, "Size mismatch");

void R_CalcCubeMapViewValues(refdef_s *refdef, CubemapShot cubemapShot, int cubemapSize);
void R_GenerateReflectionRawData(DiskGfxReflectionProbe* probeRawData);
void R_GenerateReflectionRawDataAll(DiskGfxReflectionProbe *probeRawData, int probeCount, bool *generateProbe);
bool R_CopyReflectionsFromLumpData(DiskGfxReflectionProbe *probeRawData, DiskGfxReflectionProbe *probeRawLumpData, const int lumpProbeCount);
void R_GenerateReflections(const char *mapname, GfxReflectionProbe *probes, const unsigned int probeCount);
bool R_ReflectionProbeGenerateExitWhenDone();
void R_GenerateReflectionImages(GfxReflectionProbe *probes, DiskGfxReflectionProbe *probeRawData, const int probeCount, const int probeBaseIndex);

void hk_R_GenerateReflections();