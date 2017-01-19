#include "stdafx.h"

#if USE_LEGACY_HDR

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
			DiskLightGridSampleColors_HDR[index][y][x] = (WORD)(floats[y][x] * g_LGI);
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

#endif

void __cdecl AdjustLightingContrast(int sampleCount, int baseIndex, vec3* colors)
{
	_asm
	{
		push colors
		push baseIndex
		mov edi, sampleCount
		mov ebx, 0x00430410
		call ebx
		add esp, 8
	}
}

// 0x00434E20
void __cdecl StoreLightingForDir(vec3* lighting, BYTE* dst)
{
	for (int i = 0; i < 56; i++)
	{
		GammaCorrectColor((float*)&lighting[i]);
	}

	AdjustLightingContrast(56, -1, lighting);

	for (int sampleIndex = 0; sampleIndex < 56; sampleIndex++)
	{
		vec3 v = lighting[sampleIndex] * 0.5;
		dst[sampleIndex * 3 + 0] = EncodeFloatInByte(v.r);
		dst[sampleIndex * 3 + 1] = EncodeFloatInByte(v.g);
		dst[sampleIndex * 3 + 2] = EncodeFloatInByte(v.b);
	}
}

void CalculateClusterMeanAndVariance(GridColorsCluster *Cluster)
{
	ASSERT(Cluster);
	ASSERT(Cluster->count);

	static DWORD dwCall = 0x00434320;

	__asm
	{
		mov edi, Cluster
			call[dwCall]
	}
}

GridColorsCluster *ChooseClusterToSplit()
{
	static DWORD dwCall = 0x00433090;

	__asm
	{
		call[dwCall]
	}
}

void SplitCluster(GridColorsCluster *Cluster)
{
	ASSERT(Cluster);
	ASSERT(Cluster->count >= 2);

	static DWORD dwCall = 0x00434B50;

	__asm
	{
		mov esi, Cluster
			call[dwCall]
	}
}

void SwapClusters(int FromIndex, int ToIndex)
{
	static DWORD dwCall = 0x00432EF0;

	__asm
	{
		push FromIndex
			push ToIndex
			call[dwCall]
			add esp, 0x8
	}
}

int GetClusterDefaultScore(GridColorsCluster *Cluster)
{
	static DWORD dwCall = 0x00432E90;

	__asm
	{
		mov ecx, Cluster
			call[dwCall]
	}
}

void CalculateClusterMean(GridColorsCluster *Cluster, float *Means)
{
	static DWORD dwCall = 0x004331A0;

	__asm
	{
		mov ecx, Cluster
		mov eax, Means
		call[dwCall]
	}
}

void SetLightGridColorsForCluster(GridColorsCluster *Cluster, GfxLightGridColors *Colors)
{
	float means[168];
	CalculateClusterMean(Cluster, means);

	int v2 = 0;
	do
	{
		float v3 = means[v2++];
		int v6 = (int)(v3 + 0.5);

		Colors->all[v2 - 1] = v6;
	} while (v2 < 168);
}

void __cdecl ImproveLightGridValues(int threadCount)
{
	_asm
	{
		mov eax, threadCount
		mov ebx, 0x00435830
		call ebx
	}
}

void ClusterLightGridValues(int ThreadCount)
{
	lightGridGlob->mapping = new unsigned int[lightGridGlob->pointCount];

	if (!lightGridGlob->mapping)
		Com_FatalError("Couldn't allocate %i bytes for light grid color mapping", sizeof(int) * lightGridGlob->pointCount);

	lightGridGlob->clusters = new GridColorsCluster[lightGridColorCount];

	if (!lightGridGlob->clusters)
		Com_FatalError("Couldn't allocate %i bytes for light grid colors", sizeof(GridColorsCluster) * lightGridColorCount);

	for (unsigned int i = 0; i < lightGridGlob->pointCount; ++i)
		lightGridGlob->mapping[i] = i;

	lightGridGlob->clusterCount = 1;
	lightGridGlob->clusters[0].first = 0;
	lightGridGlob->clusters[0].count = lightGridGlob->pointCount;
	CalculateClusterMeanAndVariance(&lightGridGlob->clusters[0]);

	unsigned int j = lightGridGlob->clusterCount;
	for (; lightGridGlob->clusterCount < lightGridColorCount; j = lightGridGlob->clusterCount)
	{
		GridColorsCluster *cluster = ChooseClusterToSplit();

		if (*(float *)0x153C902C >= cluster->unknown3 && j >= 2)
			break;

		SplitCluster(cluster);
	}

	unsigned int maxScore = 0;
	unsigned int maxScoreIndex = 0;

	if (j > 0)
	{
		GfxLightGridColors *lightGridColors = (GfxLightGridColors *)0x96CAE08;

		for (unsigned int i = 0; i < lightGridGlob->clusterCount; i++)
		{
			GridColorsCluster *cluster = &lightGridGlob->clusters[i];
			unsigned int firstIndex = cluster->first;

			if (firstIndex < (firstIndex + cluster->count))
			{
				do
					*(WORD *)(lightGridGlob->points + 12 * lightGridGlob->mapping[firstIndex++] + 8) = i;
				while (firstIndex < (firstIndex + cluster->count));
			}

			SetLightGridColorsForCluster(cluster, lightGridColors);

			unsigned int score = GetClusterDefaultScore(cluster);
			if (score > maxScore)
			{
				maxScore = score;
				maxScoreIndex = i;
			}

			lightGridColors++;
		}
	}

	SwapClusters(0, maxScoreIndex);
	SwapClusters(1, *(WORD *)(lightGridGlob->points + 12 * lightGridGlob->pointCount - 4));
	lightGridColorCount = lightGridGlob->clusterCount;

	if (options_ImproveLights)
		ImproveLightGridValues(ThreadCount);

	delete[] lightGridGlob->clusters;
	delete[] lightGridGlob->mapping;
	lightGridGlob->clusters = nullptr;
	lightGridGlob->mapping = nullptr;

	--lightGridGlob->pointCount;
}
