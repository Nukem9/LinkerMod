#pragma once

#include "r_lightgrid.h"

extern vec4* LightmapBytes_HDR;
extern vec4* Lightmap2Bytes_HDR;

extern SampleColorHDR* LightGridSampleColors_HDR;
extern DiskSampleColorHDR* DiskLightGridSampleColors_HDR;

void PatchHDR_Lightmaps();
void PatchHDR_Lightgrid();

void R_Init_LightmapsHDR();
void R_Init_LightgridHDR();
void R_Init_DiskLightgridHDR();

void __cdecl R_StoreLightmapPixelHDR(vec4* pel1, vec4* pel2, int lightmap, int row, int pel);