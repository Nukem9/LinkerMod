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

BYTE __cdecl EncodeFloatInByte(float flt)
{
	double f = flt;
	if (f < 0.0)
	{
		return 0;
	}

	if (f > 1.0)
	{
		return 255;
	}

	return (BYTE)(f * 255.0);
}

#if USE_LEGACY_HDR
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

//
// Currently only pel1 appears to have correct alpha data
//
void __cdecl R_StoreLightmapPixelHDR(vec4* pel1, vec4* pel2, int lightmap, int row, int pel)
{
	LightmapBytes_HDR[0x40000 * lightmap + 512 * row + pel] = *pel1;
	Lightmap2Bytes_HDR[0x40000 * lightmap + 512 * row + pel] = *pel2;
}

void* ImproveLightingApproximation = (void*)0x4323E0;
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

		call ImproveLightingApproximation
		xorps xmm1, xmm1
		jmp rtn_R_StoreLightmapPixel2
	}
}
#else

void __cdecl GetInitialLightingHighlightDir(float* lighting, float* highlightDir)
{
	_asm
	{
		pushad
		push highlightDir
		mov eax, lighting
		mov ebx, 0x00431980
		call ebx
		add esp, 4
		popad
	}
}

void __cdecl GetColorsForHighlightDir(float* lighting, float* highlightDir, float* pel1, float* pel2)
{
	_asm
	{
		pushad
		push pel2
		push pel1
		mov ecx, highlightDir
		mov eax, lighting
		mov ebx, 0x00430EF0
		call ebx
		add esp, 8
		popad
	}
}

void __cdecl ImproveLightingApproximation(float* lighting, float* highlightDir, float* pel1, float* pel2)
{
	_asm
	{
		pushad
		push pel2
		push pel1
		push highlightDir
		mov edi, lighting
		mov ebx, 0x004323E0
		call ebx
		add esp, 12
		popad
	}
}

void __declspec(naked) hk_StoreLightBytes()
{
	_asm
	{
		push[esp+12] //pFloats			esp + 12 + 0
		push[esp+12] //highlightDir		esp + 8 + 4
		push[esp+12] //pixelIndex		esp + 4 + 8
		push ecx // lmapRow
		push eax // lmapSet
		call StoreLightBytes
		add esp, 20
		retn
	}
}

//
// 'lighting' and 'highlightDir' might be mixed up atm (the names themselves) - but it appears to work correctly
//
void __cdecl StoreLightBytes(int lmapSet, int lmapRow, int pixelIndex, float* highlightDir, float* pFloats)
{
	int subOffset = 0x600 * lmapSet + lmapRow;

	float lighting[3];
	GetInitialLightingHighlightDir(lighting, highlightDir);

	float pel1[4];
	float pel2[4];
	GetColorsForHighlightDir(lighting, highlightDir, pel1, pel2);
	ImproveLightingApproximation(lighting, highlightDir, pel1, pel2);

	BYTE* lightBytes = (BYTE*)0x00471590;
	WAW_LMAP_PEL* pel = (WAW_LMAP_PEL*)&lightBytes[4 * (pixelIndex + (subOffset << 9))];

	pel->B = EncodeFloatInByte(pel1[2]);
	pel->G = EncodeFloatInByte(pel1[1]);
	pel->R = EncodeFloatInByte(pel1[0]);
	pel->A = EncodeFloatInByte(highlightDir[0] / highlightDir[2] * 0.25f + 0.5f);

	pel += 0x40000;

	pel->B = EncodeFloatInByte(pel2[2]);
	pel->G = EncodeFloatInByte(pel2[1]);
	pel->R = EncodeFloatInByte(pel2[0]);
	pel->A = EncodeFloatInByte(highlightDir[1] / highlightDir[2] * 0.25f + 0.5f);

	if (g_HDR)
	{
		vec4* out1 = &LightmapBytes_HDR[0x40000 * lmapSet + 512 * lmapRow + pixelIndex];
		out1->r = pel1[0];
		out1->g = pel1[1];
		out1->b = pel1[2];
		out1->a = highlightDir[0] / highlightDir[2] * 0.25f + 0.5f;

		vec4* out2 = &Lightmap2Bytes_HDR[0x40000 * lmapSet + 512 * lmapRow + pixelIndex];
		out2->r = pel2[0];
		out2->g = pel2[1];
		out2->b = pel2[2];
		out2->a = highlightDir[1] / highlightDir[2] * 0.25f + 0.5f;
	}

	BYTE* lightBytes_PrimaryImage = (BYTE*)0x00671590;

	BYTE* sm_pel = &lightBytes_PrimaryImage[2 * (pixelIndex + (subOffset << 10))];
	for (int i = 2; i != 0; i--)
	{
		for (int j = 2; j != 0; j--)
		{
			*sm_pel++ = EncodeFloatInByte(*pFloats);
		}

		sm_pel += 1022;
	}
}

#endif
