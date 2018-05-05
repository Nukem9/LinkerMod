#include "stdafx.h"

GfxMetrics* gfxMetrics = (GfxMetrics*)0x0396A4D4;

// /gfx_d3d/r_screenshot.cpp:1458
void R_BeginCubemapShot(const int pixelWidthHeight, const int pixelBorder)
{
	gfxMetrics->cubemapShotRes = pixelWidthHeight;
	gfxMetrics->cubemapShotPixelBorder = pixelBorder;
	R_CubemapShotSetInitialState();
}

// /gfx_d3d/r_screenshot.cpp:1577
void R_CreateReflectionRawDataFromCubemapShot(DiskGfxReflectionProbe *probeRawData)
{
	((void(__cdecl *)(DiskGfxReflectionProbe *))0x007088E0)(probeRawData);
}