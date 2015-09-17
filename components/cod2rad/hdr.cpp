#include "stdafx.h"

vec4* LightmapBytes_HDR = nullptr;
vec4* Lightmap2Bytes_HDR = nullptr;

void R_Init_LightmapsHDR()
{
	LightmapBytes_HDR = new vec4[512 * 512 * *g_LightmapCount];
	Lightmap2Bytes_HDR = new vec4[512 * 512 * *g_LightmapCount];
}

void __cdecl R_StoreLightmapPixelHDR(vec4* pel1, vec4* pel2, int lightmap, int row, int pel)
{
	LightmapBytes_HDR[0x40000 * lightmap + 512 * row + pel] = *pel1;
	Lightmap2Bytes_HDR[0x40000 * lightmap + 512 * row + pel] = *pel2;
}

void PatchHDR_Lightmaps()
{
	//
	// Remove HDR Clamping when getting lightmap pixels
	//
	PatchMemory(0x00431186, (PBYTE)"\xEB", 1);
	PatchMemory(0x004311AB, (PBYTE)"\xEB", 1);
	PatchMemory(0x004311D0, (PBYTE)"\xEB", 1);
	PatchMemory(0x00431261, (PBYTE)"\xEB", 1);
	PatchMemory(0x00431288, (PBYTE)"\xEB", 1);
	PatchMemory(0x004312AD, (PBYTE)"\xEB", 1);
	PatchMemory(0x0043133F, (PBYTE)"\xEB", 1);
	PatchMemory(0x00431360, (PBYTE)"\xEB", 1);
	PatchMemory(0x00431388, (PBYTE)"\xEB", 1);

	//
	// Remove HDR Pixel Clamping Before Converting to RGB8
	//
	PatchMemory(0x004328AA, (PBYTE)"\xEB", 1);
	PatchMemory(0x004328EC, (PBYTE)"\xEB", 1);
	PatchMemory(0x0043292F, (PBYTE)"\xEB", 1);
	PatchMemory(0x00432986, (PBYTE)"\xEB", 1);
	PatchMemory(0x004329C8, (PBYTE)"\xEB", 1);
	PatchMemory(0x00432A0B, (PBYTE)"\xEB", 1);
	PatchMemory(0x00432A86, (PBYTE)"\xEB", 1);

	//
	// Enable HDR Lighmap Allocation and Storage
	//
	o_R_BuildFinalLightmaps = (R_BuildFinalLightmaps_t)Detours::X86::DetourFunction((PBYTE)0x00432D70, (PBYTE)&hk_R_BuildFinalLightmaps);
	Detours::X86::DetourFunction((PBYTE)0x00432830, (PBYTE)&hk_R_StoreLightmapPixel);
	Detours::X86::DetourFunction((PBYTE)0x00432885, (PBYTE)&mfh_R_StoreLightmapPixel);
}
