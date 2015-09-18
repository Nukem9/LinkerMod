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

void __cdecl R_Store_LightGridSample_HDR(int sampleIndex, int pelIndex, int channelIndex, float value)
{
	LightGridSampleColors_HDR[sampleIndex][pelIndex][channelIndex] = value;
}

void * ag = (void*)0x00434E8F;
void* rtn_R_Store_LightgridSample = (void*)0x00434E87;
void __declspec(naked) mfh_R_Store_LightgridSample()
{
	_asm
	{
		push eax
		push ebx
		mov eax, [esp + 34h]
		mov ebx, [esp + 18h]
		pushad
		push ebx //value
		push ecx //channelIndex

		sub edi, 1
		push edi //pelIndex

		//Calculate the index based on the offset and entry size
		mov ebx, 153C91DCh
		sub eax, [ebx]
		mov edx, 0
		mov ebx, 168
		div ebx

		push eax //sampleIndex
		call R_Store_LightGridSample_HDR
		add esp, 16
		popad
		pop ebx
		pop eax

		movss xmm1, [esp + 10h]
		comiss xmm1, xmm0
	
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
	double sums[56][3];
	memset(sums, 0, sizeof(double) * 168);

	DWORD LGCCount = LGCBuf->d1_lightgridcolorcount;

	if (LGCCount)
	{
		SampleColorHDR* originalColors = LightGridSampleColors_HDR;
		DWORD* dw_lightGridColorMapping = *(DWORD**)0x153C91E8;
		DWORD* lightGridColorIndex = &dw_lightGridColorMapping[LGCBuf->d0_index];

		//
		// Calculate Sums
		//
		for (DWORD colorIndex = 0; colorIndex < LGCCount; colorIndex++)
		{
			SampleColorHDR* originalEntry = &originalColors[*lightGridColorIndex];

			for (int y = 0; y < 56; y++)
			{
				for (int x = 0; x < 3; x++)
				{
					float value = (*originalEntry)[y][x] < 0 ? 0.0f : (float)(*originalEntry)[y][x];
					sums[y][x] += value;
				}
			}

			lightGridColorIndex++;
		}
	}

	double divisor = LGCCount;
	SampleColorHDR* dest = (SampleColorHDR*)floats;

	for (int y = 0; y < 56; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			(*dest)[y][x] = (float)(sums[y][x] / divisor);
		}
	}
}

void __cdecl R_Store_QuantizedLightGridSampleHDR(GfxLightGridColor_Internal* LGCBuf_Internal, BYTE* dest)
{
	SampleColorHDR floats;
	BYTE result = 0;
	R_QuantizeLightGridColorHDR((float*)&floats, LGCBuf_Internal);

	DWORD index = ((DWORD)dest - 0x096CAE08) / (168);

	for (int y = 0; y < 56; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			DiskLightGridSampleColors_HDR[index][y][x] = (WORD)(floats[y][x] * 255.0f * 8.0f + 0.5f);
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