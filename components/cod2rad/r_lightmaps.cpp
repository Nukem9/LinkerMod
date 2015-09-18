#include "stdafx.h"

R_BuildFinalLightmaps_t o_R_BuildFinalLightmaps = (R_BuildFinalLightmaps_t)0x00432D70;

void __declspec(naked) hk_R_BuildFinalLightmaps()
{
	_asm
	{
		pushad
		call R_Init_LightmapsHDR
		popad
		jmp o_R_BuildFinalLightmaps
		retn
	}
}

//
// Store pixel information for later use by mfh_R_StoreLightmapPixel
//
void* rtn_R_StoreLightmapPixel1 = (void*)0x00432836;
void __declspec(naked) hk_R_StoreLightmapPixel() //00432830
{
	_asm
	{
		push ebp
		mov ebp, esp
		and esp, 0FFFFFFF8h

		movd xmm3, eax //lightmap
		movd xmm4, ecx //row
		movd xmm5, [ebp + 8] //pixel

		jmp rtn_R_StoreLightmapPixel1
	}
}

void __cdecl R_StoreLightmapPixelHDR(vec4* pel1, vec4* pel2, int lightmap, int row, int pel)
{
	LightmapBytes_HDR[0x40000 * lightmap + 512 * row + pel] = *pel1;
	Lightmap2Bytes_HDR[0x40000 * lightmap + 512 * row + pel] = *pel2;
}

void* sub_4323E0 = (void*)0x4323E0;
void* rtn_R_StoreLightmapPixel2 = (void*)0x0043288D;
void __declspec(naked) mfh_R_StoreLightmapPixel()
{
	_asm
	{
		pushad
		movd ebx, xmm5
		push ebx	//pel
		movd ecx, xmm4
		push ecx	//row
		movd ebx, xmm3
		push ebx	//lightmap

		push eax	//pel2
		push edx	//pel1
		call R_StoreLightmapPixelHDR
		add esp, 20
		popad

		call sub_4323E0
		xorps xmm1, xmm1
		jmp rtn_R_StoreLightmapPixel2
	}
}