#include <WTypes.h>
#include "Lump.h"

struct DiskGfxLightGridColors_WAW
{
  BYTE rgb[56][3];
};

struct DiskGfxLightGridColors_BO
{
  WORD rgb[56][3];
};

int ConvertLump_WAWtoBO_LightGridColors(Lump* wawLump, Lump* boLump);