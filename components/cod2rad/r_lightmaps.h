#pragma once

const static DWORD* g_LightmapCount = (DWORD*)0x16E99F58;

typedef void(*R_BuildFinalLightmaps_t)();
extern R_BuildFinalLightmaps_t o_R_BuildFinalLightmaps;

void hk_R_BuildFinalLightmaps();

BYTE __cdecl EncodeFloatInByte(float flt);

#if USE_LEGACY_HDR
void hk_R_StoreLightmapPixel();
void mfh_R_StoreLightmapPixel();
#else
void hk_StoreLightBytes();
void __cdecl StoreLightBytes(int lmapSet, int lmapRow, int pixelIndex, float* highlightDir, float* pFloats);
#endif