#pragma once

extern vec4* LightmapBytes_HDR;
extern vec4* Lightmap2Bytes_HDR;


void PatchHDR_Lightmaps();

void R_Init_LightmapsHDR();
void __cdecl R_StoreLightmapPixelHDR(vec4* pel1, vec4* pel2, int lightmap, int row, int pel);