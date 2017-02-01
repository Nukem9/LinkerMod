#pragma once
#include "stdafx.h"

void* R_DrawXModelSkinnedCached_o = nullptr;
void* R_DrawXModelSkinnedUncached_o = nullptr;

R_DrawXModelRigidModelSurf_t R_DrawXModelRigidModelSurf_o = (R_DrawXModelRigidModelSurf_t)0x0;

void __cdecl R_WorldMatrixChanged(GfxCmdBufSourceState *source)
{
	++source->matrixVersions[0];
	++source->matrixVersions[3];
	++source->matrixVersions[5];
	++source->matrixVersions[7];

	source->constVersions[194] = source->matrixVersions[0];
}

GfxMatrix *__cdecl R_GetActiveWorldMatrix(GfxCmdBufSourceState *source)
{
	R_WorldMatrixChanged(source);
	return source->matrices.matrix;
}

void __declspec(naked) hk_R_DrawXModelSkinnedCached(GfxCmdBufContext context) // 0x0073BF30
{
	_asm
	{
		push eax // modelSurf
		push[esp + 12] // context.state
		push[esp + 12] // context.source
		call R_DrawXModelSkinnedCached
		add esp, 12
		retn
	}
}

void __cdecl R_DrawXModelSkinnedCached(GfxCmdBufContext context, struct GfxModelSkinnedSurface *modelSurf)
{
	GfxCmdBufSourceState* source = context.source;
	GfxCmdBufState* state = context.state;

	DBG_ASSERT(R_DrawXModelSkinnedCached_o);

	_asm
	{
		push state  // [&context+4]
		push source // [&context]
		mov eax, modelSurf
		call R_DrawXModelSkinnedCached_o
		add esp, 8
	}

	if (r_showTess->current.enabled)
	{
		GfxMatrix* mtx = R_GetActiveWorldMatrix(context.source);
		RB_ShowTess(context, (float*)&mtx->m[3], "XMSkin$", colorWhite);
	}
}

void __declspec(naked) hk_R_DrawXModelSkinnedUncached() // 0x0073BFC0
{
	_asm
	{
		push[esp + 12] // skinnedVert
		push eax // xsurf
		push[esp + 16] // context.state
		push[esp + 16] // context.source
		call R_DrawXModelSkinnedUncached
		add esp, 16
		retn
	}
}

void __cdecl R_DrawXModelSkinnedUncached(GfxCmdBufContext context, XSurface *xsurf, GfxPackedVertex *skinnedVert)
{
	GfxCmdBufSourceState* source = context.source;
	GfxCmdBufState* state = context.state;

	DBG_ASSERT(R_DrawXModelSkinnedUncached_o);

	_asm
	{
		push skinnedVert
		push state  // [&context+4]
		push source // [&context]
		mov eax, xsurf
		call R_DrawXModelSkinnedUncached_o
		add esp, 12
	}

	if (r_showTess->current.enabled)
	{
		GfxMatrix* mtx = R_GetActiveWorldMatrix(context.source);
		RB_ShowTess(context, (float*)&mtx->m[3], "XMSkinUn$", colorWhite);
	}
}

void __cdecl hk_R_DrawXModelRigidModelSurf(XSurface *xsurf, GfxCmdBufContext context)
{
	R_DrawXModelRigidModelSurf(context, xsurf);
}

void __cdecl mfh_R_TessXModelWaterList_ShowTess(GfxCmdBufContext context)
{
	MessageBoxA(0, "R_TessXModelWaterList", 0, 0); // Todo: Confirm that this func even runs

	if (r_showTess->current.enabled)
	{
		GfxMatrix* mtx = R_GetActiveWorldMatrix(context.source);
		RB_ShowTess(context, (float*)&mtx->m[3], "XMRigid", colorWhite);
	}
}

void* rtn_R_TessXModelWaterList = (void*)0x0073D8B8;
void __declspec(naked) mfh_R_TessXModelWaterList(void) // 0x0073D8B2
{
	_asm
	{
		call eax
		mov eax, [esp + 0x14]
		push esi // context.state
		push ebx // context.source
		call mfh_R_TessXModelWaterList_ShowTess
		add esp, 8

		jmp rtn_R_TessXModelWaterList
	}
}

void __cdecl R_DrawXModelRigidModelSurf(GfxCmdBufContext context, XSurface *xsurf)
{
	DBG_ASSERT(R_DrawXModelRigidModelSurf_o);
	R_DrawXModelRigidModelSurf_o(xsurf, context);

	if (r_showTess->current.enabled)
	{
		GfxMatrix* mtx = R_GetActiveWorldMatrix(context.source);
		RB_ShowTess(context, (float*)&mtx->m[3], "XMRigid", colorWhite);
	}
}
