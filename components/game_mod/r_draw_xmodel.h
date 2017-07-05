#pragma once
#include "r_scene.h"

extern void* R_DrawXModelSkinnedCached_o;
extern void* R_DrawXModelSkinnedUncached_o;

typedef void(__cdecl* R_DrawXModelRigidModelSurf_t)(struct XSurface *xsurf, GfxCmdBufContext context);
extern R_DrawXModelRigidModelSurf_t R_DrawXModelRigidModelSurf_o;

void hk_R_DrawXModelSkinnedCached(GfxCmdBufContext context);
void hk_R_DrawXModelSkinnedUncached(void);
void hk_R_DrawXModelRigidModelSurf(XSurface *xsurf, GfxCmdBufContext context);

void mfh_R_TessXModelWaterList(void);

void __cdecl R_DrawXModelSkinnedCached(GfxCmdBufContext context, struct GfxModelSkinnedSurface *modelSurf);
void __cdecl R_DrawXModelSkinnedUncached(GfxCmdBufContext context, XSurface *xsurf, struct GfxPackedVertex *skinnedVert);
void __cdecl R_DrawXModelRigidModelSurf(GfxCmdBufContext context, XSurface *xsurf);
