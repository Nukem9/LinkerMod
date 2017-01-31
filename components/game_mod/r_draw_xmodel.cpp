#pragma once
#include "stdafx.h"

void* R_DrawXModelSkinnedCached_o = nullptr;
void* R_DrawXModelSkinnedUncached_o = nullptr;

R_DrawXModelRigidModelSurf_t R_DrawXModelRigidModelSurf_o = (R_DrawXModelRigidModelSurf_t)0x0;

//GfxMatrix *__cdecl R_GetActiveWorldMatrix(GfxCmdBufSourceState *source)
//{
//	R_WorldMatrixChanged(source);
//	return source->matrices.matrix;
//}

GfxCmdBufSourceState *__cdecl R_GetActiveWorldMatrix(GfxCmdBufSourceState *source)
{
	GfxCmdBufSourceState *result = source;
	++result->matrixVersions[0];
	unsigned __int16 val = source->matrixVersions[0];
	++result->matrixVersions[3];
	++result->matrixVersions[5];
	++result->matrixVersions[7];
	source->constVersions[221] = val;
	return result;
}

void __declspec(naked) hk_R_DrawXModelSkinnedCached(GfxCmdBufContext context) // 0x0073BF30
{
	_asm
	{
		push eax // modelSurf
			push[esp + 12] // context.local._s0.state
			push[esp + 12] // context.local._s0.source
			call R_DrawXModelSkinnedCached
			add esp, 12
			retn
	}
}

void __cdecl R_DrawXModelSkinnedCached(GfxCmdBufContext context, struct GfxModelSkinnedSurface *modelSurf)
{
	GfxCmdBufSourceState* source = context.local._s0.source;
	GfxCmdBufState* state = context.local._s0.state;

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
		GfxMatrix* mtx = R_GetActiveWorldMatrix(context.local._s0.source)->matrices.matrix;
		RB_ShowTess(context, (float*)&mtx->m[3], "XMSkin$", colorWhite);
	}
}

//
// Visual studio is loading these args backwards for some reason
//
void __declspec(naked) hk_R_DrawXModelSkinnedUncached1(GfxCmdBufContext context, GfxPackedVertex* skinnedVert) // 0x0073BFC0
{
	//_asm
	//{
	//	push[esp+12]
	//	push[esp+12]
	//	push[esp+12]
	//	call R_DrawXModelSkinnedUncached_o
	//	add esp, 12
	//	retn
	//}

	_asm
	{
		push[esp + 12]
			push[esp + 12]
			push skinnedVert
			call R_DrawXModelSkinnedUncached_o
			add esp, 12
			retn
	}

	_asm
	{
		push skinnedVert
			push eax // xsurf
			push[esp + 20] // context.local._s0.state
			push[esp + 20] // context.local._s0.source
			call R_DrawXModelSkinnedUncached
			add esp, 16
			retn
	}
}

void __declspec(naked) hk_R_DrawXModelSkinnedUncached() // 0x0073BFC0
{
	//_asm
	//{
	//	push[esp + 12]
	//	push[esp + 12]
	//	push[esp + 12]
	//	call R_DrawXModelSkinnedUncached_o
	//	add esp, 12
	//	retn
	//}

	_asm
	{
		push[esp + 12] // skinnedVert
			push eax // xsurf
			push[esp + 20] // context.local._s0.state
			push[esp + 20] // context.local._s0.source
			call R_DrawXModelSkinnedUncached
			add esp, 16
			retn
	}
}

void __cdecl R_DrawXModelSkinnedUncached(GfxCmdBufContext context, XSurface *xsurf, GfxPackedVertex *skinnedVert)
{
	GfxCmdBufSourceState* source = context.local._s0.source;
	GfxCmdBufState* state = context.local._s0.state;

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
		GfxMatrix* mtx = R_GetActiveWorldMatrix(context.local._s0.source)->matrices.matrix;
		RB_ShowTess(context, (float*)mtx->m[3], "XMSkinUn$", colorWhite);
	}
}

void __cdecl hk_R_DrawXModelRigidModelSurf(XSurface *xsurf, GfxCmdBufContext context)
{
	R_DrawXModelRigidModelSurf(context, xsurf);
}

void __cdecl R_DrawXModelRigidModelSurf(GfxCmdBufContext context, XSurface *xsurf)
{
	DBG_ASSERT(R_DrawXModelRigidModelSurf_o);
	R_DrawXModelRigidModelSurf_o(xsurf, context);

	if (r_showTess->current.enabled)
	{
		GfxMatrix* mtx = R_GetActiveWorldMatrix(context.local._s0.source)->matrices.matrix;
		RB_ShowTess(context, (float*)mtx->m[3], "XMRigid", colorWhite);
	}
}
