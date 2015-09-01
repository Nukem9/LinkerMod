#pragma once

void R_BeginCubemapShot(const int pixelWidthHeight, const int pixelBorder);

typedef int __cdecl R_CubemapShotSetInitialState_t(void);
static R_CubemapShotSetInitialState_t* R_CubemapShotSetInitialState = (R_CubemapShotSetInitialState_t*)0x00708710;

typedef void __cdecl R_EndCubemapShot_t(CubemapShot shotIndex);
static R_EndCubemapShot_t* R_EndCubemapShot = (R_EndCubemapShot_t*)0x00708890;
