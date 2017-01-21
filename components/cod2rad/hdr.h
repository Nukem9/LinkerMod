#pragma once

#include "r_lightgrid.h"

extern vec4* LightmapBytes_HDR;
extern vec4* Lightmap2Bytes_HDR;

#if USE_LEGACY_HDR
extern SampleColorHDR* LightGridSampleColors_HDR;
extern DiskSampleColorHDR* DiskLightGridSampleColors_HDR;
#endif

void PatchHDR_Lightmaps();
void PatchHDR_Lightgrid();

void R_Init_LightmapsHDR();
#if USE_LEGACY_HDR
void R_Init_DiskLightgridHDR();
#endif

void __cdecl R_StoreLightmapPixelHDR(vec4* pel1, vec4* pel2, int lightmap, int row, int pel);