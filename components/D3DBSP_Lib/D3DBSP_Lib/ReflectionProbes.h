#include "Lump.h"

struct DiskGfxReflectionProbeVolumeData
{
  float volumePlanes[6][4];
};
struct DiskGfxReflectionProbe_BO
{
  float origin[3];
  BYTE pixels[262224]; //A 256x256 RGBA CubeMap with MipMaps (DXT1)
  DiskGfxReflectionProbeVolumeData probeVolumes[16];
  unsigned int probeVolumeCount;
  char colorCorrectionFilename[64];
  char name[64];
};

struct DiskGfxReflectionProbe_WAW
{
  float origin[3];
  char colorCorrectionFilename[64];
  BYTE pixels[131064];  //64x64 cubemap with 7 mips using uncompressed RGBA8
};

int ConvertLump_WAWtoBO_ReflectionProbes(Lump* wawLump, Lump* boLump);
int ConvertLump_BOtoWAW_ReflectionProbes(Lump* boLump, Lump* wawLump);