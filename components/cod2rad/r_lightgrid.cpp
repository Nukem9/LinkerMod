#include "stdafx.h"

R_Init_Lightgrid_t o_R_Init_Lightgrid = (R_Init_Lightgrid_t)0x00433440;
R_Store_QuantizedLightGridSample_t o_R_Store_QuantizedLightGridSample = (R_Store_QuantizedLightGridSample_t)0x00433890;

struct GfxLightGridColor_Internal
{
	DWORD d0_index;
	DWORD d1_lightgridcolorcount;
	DWORD d2;
	float f0;
	float f1;
};

void* rtn_R_Init_Lightgrid = (void*)0x0043684C;
void __declspec(naked) mfh_R_Init_Lightgrid()
{
	_asm
	{
		pushad
		call R_Init_LightgridHDR
		popad
		push eax
		mov eax, 00433440h
		call eax
		pop eax
		jmp rtn_R_Init_Lightgrid
		retn
	}
}

void __cdecl R_Store_LightGridSample_HDR(DWORD entryOffset, DWORD pixelIndex, float value)
{
	float* LightGridEntry = (float*)LightGridSampleColors_HDR;
	LightGridEntry += (entryOffset - *(DWORD*)0x153C91DC);

	LightGridEntry[pixelIndex] = value;
}

void* rtn_R_Store_LightgridSample = (void*)0x00434EB3;
void __declspec(naked) mfh_R_Store_LightgridSample()
{
	_asm
	{
		fadd [esp + 18h]
		
		push ebx
		mov ebx, [esp + 14h] 
		pushad
		push ebx //value
		push ecx //pixelIndex
		push esi //entryOffset
		call R_Store_LightGridSample_HDR
		add esp, 12
		popad
		pop ebx

		fistp [esp + 14h]
		mov	al, [esp + 14h]
		jmp rtn_R_Store_LightgridSample
	}
}

void* rtn_R_Alloc_DiskLightGridColors = (void*)0x00435C94;
void __declspec(naked) mfh_R_Alloc_DiskLightGridColors()
{
	_asm
	{
		pushad
		call R_Init_DiskLightgridHDR
		popad

		cmp edi, ebp
		mov[esp + 10h], ebp
		jmp rtn_R_Alloc_DiskLightGridColors
	}
}

void __cdecl R_QuantizeLightGridColorHDR(float* floats, GfxLightGridColor_Internal* LGCBuf)
{
	double sums[168];
	memset(sums, 0, sizeof(double) * 168);

	DWORD LGCCount = LGCBuf->d1_lightgridcolorcount;

	if (LGCCount)
	{
		float* originalColors = (float*)LightGridSampleColors_HDR;
		DWORD* dw_lightGridColorMapping = *(DWORD**)0x153C91E8;
		DWORD* lightGridColorIndex = &dw_lightGridColorMapping[LGCBuf->d0_index];

		//
		// Calculate Sums
		//
		for (DWORD colorIndex = 0; colorIndex < LGCCount; colorIndex++)
		{
			float* originalEntry = &originalColors[168 * *lightGridColorIndex];

			for (int i = 0; i < 168; i++)
			{
				sums[i] += originalEntry[i];;
			}

			lightGridColorIndex++;
		}
	}

	float divisor = (float)LGCCount;

	for (int i = 0; i < 168; i++)
	{
		floats[i] = (float)sums[i];

		if (sums[i] < 0)
			floats[i] = 0;

		floats[i] /= divisor;
	}
}

void __cdecl R_Store_QuantizedLightGridSampleHDR(GfxLightGridColor_Internal* LGCBuf_Internal, BYTE* dest)
{
	SampleColorHDR floats;
	R_QuantizeLightGridColorHDR(&floats[0][0], LGCBuf_Internal);

	DWORD index = ((DWORD)dest - 0x096CAE08) / (168);

	for (int y = 0; y < 56; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			DiskLightGridSampleColors_HDR[index][y][x] = (WORD)(floats[y][x]);
		}
	}
}

void __declspec(naked) hk_R_Store_QuantizedLightGridSample()
{
	_asm
	{
		pushad
		push esi //destination
		push ecx //LGCBuf_internal
		call R_Store_QuantizedLightGridSampleHDR
		add esp, 8
		popad
		call o_R_Store_QuantizedLightGridSample
		retn
	}
}