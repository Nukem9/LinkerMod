#pragma once

static_assert(sizeof(D3DFORMAT) == sizeof(DWORD), "D3DFORMAT must be 4 bytes!");

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
	D3DFORMAT shadowmapFormatPrimary;
	D3DFORMAT shadowmapFormatSecondary;
	char shadowmapBuildTechType;
	char shadowmapSamplerState;
	bool slopeScaleDepthBias;
	bool canMipCubemaps;
	bool hasTransparencyMsaa;
};

void R_BeginCubemapShot(const int pixelWidthHeight, const int pixelBorder);
void R_CreateReflectionRawDataFromCubemapShot(DiskGfxReflectionProbe *probeRawData);

typedef int __cdecl R_CubemapShotSetInitialState_t(void);
static R_CubemapShotSetInitialState_t* R_CubemapShotSetInitialState = (R_CubemapShotSetInitialState_t*)0x00708710;

typedef void __cdecl R_EndCubemapShot_t(CubemapShot shotIndex);
static R_EndCubemapShot_t* R_EndCubemapShot = (R_EndCubemapShot_t*)0x00708890;
