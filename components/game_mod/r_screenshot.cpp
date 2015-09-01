#include "stdafx.h"
//DWORD (__cdecl *R_CubemapShotSetInitialState)() = (DWORD (__cdecl*)())0x00708710;
//void (__cdecl *R_EndCubemapShot)(int) = (void (__cdecl*)(int))0x00708890;

typedef DWORD _D3DFORMAT;

struct GfxMetrics
{
	unsigned __int16 cubemapShotRes;
	unsigned __int16 cubemapShotPixelBorder;
	unsigned __int16 feedbackWidth;
	unsigned __int16 feedbackHeight;
	bool hasAnisotropicMinFilter;
	bool hasAnisotropicMagFilter;
	int maxAnisotropy;
	int maxClipPlanes;
	bool hasHardwareShadowmap;
	_D3DFORMAT shadowmapFormatPrimary;
	_D3DFORMAT shadowmapFormatSecondary;
	char shadowmapBuildTechType;
	char shadowmapSamplerState;
	bool slopeScaleDepthBias;
	bool canMipCubemaps;
	bool hasTransparencyMsaa;
};

GfxMetrics* gfxMetrics = (GfxMetrics*)0x0396A4D4;

void R_BeginCubemapShot(const int pixelWidthHeight, const int pixelBorder)
{
	gfxMetrics->cubemapShotRes = pixelWidthHeight;
	gfxMetrics->cubemapShotPixelBorder = pixelBorder;
	R_CubemapShotSetInitialState();
}