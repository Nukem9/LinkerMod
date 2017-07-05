#include "stdafx.h"

vec4* LightmapBytes_HDR = nullptr;
vec4* Lightmap2Bytes_HDR = nullptr;

#if USE_LEGACY_HDR
SampleColorHDR* LightGridSampleColors_HDR = nullptr;
DiskSampleColorHDR* DiskLightGridSampleColors_HDR = nullptr;
#endif

void R_Init_LightmapsHDR()
{
	LightmapBytes_HDR = new vec4[512 * 512 * *g_LightmapCount];
	Lightmap2Bytes_HDR = new vec4[512 * 512 * *g_LightmapCount];
}

#if USE_LEGACY_HDR
void R_Init_LightgridHDR()
{
	LightGridSampleColors_HDR = new SampleColorHDR[*g_lightgridSampleCount + 1];
	memset(LightGridSampleColors_HDR, 0, sizeof(SampleColorHDR) * (*g_lightgridSampleCount + 1));
}

void R_Init_DiskLightgridHDR()
{
	DiskLightGridSampleColors_HDR = new DiskSampleColorHDR[*g_diskLightgridSampleCount];
}
#endif

void PatchHDR_Lightmaps()
{
#if USE_LEGACY_HDR
	//
	// Remove HDR Clamping when getting lightmap pixels (GetColorsForHighlightDir)
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
	// Remove HDR Pixel Clamping Before Converting to RGB8 (StoreLightBytes)
	//
	PatchMemory(0x004328AA, (PBYTE)"\xEB", 1);
	PatchMemory(0x004328EC, (PBYTE)"\xEB", 1);
	PatchMemory(0x0043292F, (PBYTE)"\xEB", 1);
	PatchMemory(0x00432986, (PBYTE)"\xEB", 1);
	PatchMemory(0x004329C8, (PBYTE)"\xEB", 1);
	PatchMemory(0x00432A0B, (PBYTE)"\xEB", 1);
	PatchMemory(0x00432A86, (PBYTE)"\xEB", 1);
#endif

	//
	// Enable HDR Lighmap Allocation and Storage
	//
	o_R_BuildFinalLightmaps = (R_BuildFinalLightmaps_t)Detours::X86::DetourFunction((PBYTE)0x00432D70, (PBYTE)&hk_R_BuildFinalLightmaps);

#if USE_LEGACY_HDR
	Detours::X86::DetourFunction((PBYTE)0x00432830, (PBYTE)&hk_R_StoreLightmapPixel);
	Detours::X86::DetourFunction((PBYTE)0x00432885, (PBYTE)&mfh_R_StoreLightmapPixel);
#endif
}

void PatchHDR_Lightgrid()
{
#if USE_LEGACY_HDR
	//
	// Remove HDR Clamping
	//
	PatchMemory(0x00434E87, (PBYTE)"\xEB", 1);
	
	// Allocates the HDR sample color buffer, runs just before SetupLightRegions()
	//
	Detours::X86::DetourFunction((PBYTE)0x00436847, (PBYTE)&mfh_R_Init_Lightgrid);

	Detours::X86::DetourFunction((PBYTE)0x00434EA7, (PBYTE)&mfh_R_Store_LightgridSample); // StoreLightingForDir - Just rewrite the whole func

	Detours::X86::DetourFunction((PBYTE)0x00435C8E, (PBYTE)&mfh_R_Alloc_DiskLightGridColors); // ClusterLightGridValues
	o_R_Store_QuantizedLightGridSample = (R_Store_QuantizedLightGridSample_t)Detours::X86::DetourFunction((PBYTE)0x00433890, (PBYTE)&hk_R_Store_QuantizedLightGridSample); // SetLightGridColorsForCluster
#endif

	//
	// Patch the struct size when allocating lightGridGlob->colors in CalculateLightGrid()
	// Makes it twice as large to accommodate for the HDR colors
	//
	// Note - lightgridglob->colors pointers in essentially everywhere its used
	//
	int size = sizeof(GfxLightGridColorsHDR); //GfxLightGridColors
	PatchMemory(0x004367C0, (PBYTE)&size, 4);

	//
	// Adjust the multiplication used to generate a pointer to a specific entry to pass to StoreLightingForDir
	// Accomodates the new struct type
	//
	PatchMemory(0x004364F3, (PBYTE)&size, 4);
	PatchMemory(0x0043561B, (PBYTE)&size, 4);

	//
	// Automatically adjust the GfxLightGridColors pointer passed to StoreLightingForDir for our HDR struct
	//
	Detours::X86::DetourFunction((PBYTE)0x00434E20, (PBYTE)&StoreLightingForDir);
	Detours::X86::DetourFunction((PBYTE)0x00435B70, (PBYTE)&ClusterLightGridValues);
}