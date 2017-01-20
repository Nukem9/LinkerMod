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
#endif

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

#if USE_LEGACY_HDR
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
		call dwCall
	}
}

GridColorsCluster *ChooseClusterToSplit()
{
	static DWORD dwCall = 0x00433090;

	__asm
	{
		call dwCall
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
		call dwCall
	}
}

int GetClusterDefaultScore(GridColorsCluster *Cluster)
{
	static DWORD dwCall = 0x00432E90;

	__asm
	{
		mov ecx, Cluster
		call dwCall
	}
}

void CalculateClusterMean(GridColorsCluster *Cluster, float *Means)
{
	static DWORD dwCall = 0x004331A0;

	__asm
	{
		mov ecx, Cluster
		mov eax, Means
		call dwCall
	}
}

void SetLightGridColorsForCluster(GridColorsCluster *Cluster, GfxLightGridColors *Colors)
{
	float means[168];
	CalculateClusterMean(Cluster, means);

	for (int i = 0; i < 168; i++)
	{
		Colors->all[i] = EncodeFloatInByte(means[i] + 0.5f);
	}
}

/*
void __cdecl ImproveLightGridValues(int threadCount)
{
	_asm
	{
		mov eax, threadCount
		mov ebx, 0x00435830
		call ebx
	}
}
*/

void __cdecl ClusterLightGridValues(int ThreadCount)
{
	lightGridGlob->mapping = new unsigned int[lightGridGlob->pointCount];
	if (!lightGridGlob->mapping)
		Com_FatalError("Couldn't allocate %i bytes for light grid color mapping", sizeof(int) * lightGridGlob->pointCount);
	
	lightGridGlob->clusters = new GridColorsCluster[LIGHTGRID_MAX_COLORCOUNT];
	if (!lightGridGlob->clusters)
		Com_FatalError("Couldn't allocate %i bytes for light grid colors", sizeof(GridColorsCluster) * LIGHTGRID_MAX_COLORCOUNT);
	
	for (unsigned int i = 0; i < lightGridGlob->pointCount; i++)
	{
		lightGridGlob->mapping[i] = i;
	}
	
	lightGridGlob->clusterCount = 1;
	lightGridGlob->clusters[0].first = 0;
	lightGridGlob->clusters[0].count = lightGridGlob->pointCount;
	
	CalculateClusterMeanAndVariance(&lightGridGlob->clusters[0]);
	
	unsigned int j = lightGridGlob->clusterCount;
	for (; lightGridGlob->clusterCount < LIGHTGRID_MAX_COLORCOUNT; j = lightGridGlob->clusterCount)
	{
		GridColorsCluster *cluster = ChooseClusterToSplit();
	
		if (cluster->unknown3 < options_clusterThreshold && j >= 2)
			break;
	
		SplitCluster(cluster);
	}
	
	unsigned int maxScore = 0;
	unsigned int maxScoreIndex = 0;
	
	if (j > 0)
	{
		GfxLightGridColors *lightGridColors = (GfxLightGridColors *)0x96CAE08;
	
		for (unsigned int colorIndex = 0; colorIndex < lightGridGlob->clusterCount; colorIndex++)
		{
			GridColorsCluster *cluster = &lightGridGlob->clusters[colorIndex];
			for (unsigned int firstIndex = cluster->first; firstIndex < (cluster->first + cluster->count); firstIndex++)
			{
				lightGridGlob->points[lightGridGlob->mapping[firstIndex]].entry.colorsIndex = colorIndex;
			}
			
			SetLightGridColorsForCluster(cluster, &lightGridColors[colorIndex]);
			
			unsigned int score = GetClusterDefaultScore(cluster);
			if (score > maxScore)
			{
				maxScore = score;
				maxScoreIndex = colorIndex;
			}
		}
	}
	
	SwapClusters(0, maxScoreIndex);
	SwapClusters(1, lightGridGlob->points[lightGridGlob->pointCount - 1].entry.colorsIndex);
	
	lightGridColorCount = lightGridGlob->clusterCount;
	
	if (options_ImproveLights)
		ImproveLightGridValues(ThreadCount);
	
	delete[] lightGridGlob->clusters;
	delete[] lightGridGlob->mapping;
	lightGridGlob->clusters = nullptr;
	lightGridGlob->mapping = nullptr;
	
	--lightGridGlob->pointCount;
}

union GfxLightGridColorSums
{
	int rgb[56][3];
	int all[56 * 3];
};

void __cdecl ImproveLightGridValues(int threadCount)
{
	GfxLightGridColorSums* sums = new GfxLightGridColorSums[lightGridGlob->clusterCount];
	if (!sums)
		Com_FatalError("Couldn't allocate %i bytes for light grid color sums", sizeof(GfxLightGridColorSums) * lightGridGlob->clusterCount);
	memset(sums, 0, sizeof(GfxLightGridColorSums) * lightGridGlob->clusterCount);

	int* counts = new int[LIGHTGRID_MAX_COLORCOUNT];
	if (!counts)
		Com_FatalError("Couldn't allocate %i bytes for light grid color counts", sizeof(int) * LIGHTGRID_MAX_COLORCOUNT);
	memset(counts, 0, sizeof(int) * lightGridGlob->clusterCount);

	BeginProgress("Improving quantization...");
	ForEachQuantum(lightGridGlob->pointCount, GuessLightGridColors, threadCount);

	//if (lightGridGlob->pointCount)
	//{
	//
	//
	//	// ???
	//	//int i = 0;
	//	//do
	//	//{
	//	//	counts[lightGridGlob->points[i].entry.colorsIndex]++;
	//	//	GfxLightGridColors* colors = *(GfxLightGridColors**)0x184C3104; // Clean this up later
	//	//	for (int sampleIndex = 0; sampleIndex < 56; sampleIndex++)
	//	//	{
	//	//		sums[lightGridGlob->points[i].entry.colorsIndex].rgb[sampleIndex][0] += colors[i].rgb[sampleIndex][0];
	//	//		sums[lightGridGlob->points[i].entry.colorsIndex].rgb[sampleIndex][1] += colors[i].rgb[sampleIndex][1];
	//	//		sums[lightGridGlob->points[i].entry.colorsIndex].rgb[sampleIndex][2] += colors[i].rgb[sampleIndex][2];
	//	//		sums[lightGridGlob->points[i].entry.colorsIndex].rgb[sampleIndex][3] += colors[i].rgb[sampleIndex][3];
	//	//	}
	//	//}
	//	//while ()
	//}

	for (unsigned int pointIndex = 0; pointIndex < lightGridGlob->pointCount; pointIndex++)
	{
		counts[lightGridGlob->points[pointIndex].entry.colorsIndex]++;
		GfxLightGridColors* colors = *(GfxLightGridColors**)0x184C3104; // Clean this up later
		for (int i = 0; i < 56; i++)
		{
			sums[lightGridGlob->points[pointIndex].entry.colorsIndex].rgb[i][0] += colors[pointIndex].rgb[i][0];
			sums[lightGridGlob->points[pointIndex].entry.colorsIndex].rgb[i][0] += colors[pointIndex].rgb[i][1];
			sums[lightGridGlob->points[pointIndex].entry.colorsIndex].rgb[i][0] += colors[pointIndex].rgb[i][2];
			sums[lightGridGlob->points[pointIndex].entry.colorsIndex].rgb[i][0] += colors[pointIndex].rgb[i][3];
		}
	}

	for (unsigned int colorIndex = 0; colorIndex < lightGridColorCount; )
	{
		if (counts[colorIndex])
		{
			GfxLightGridColors* colors = (GfxLightGridColors *)0x96CAE08;
			for (int i = 0; i < 56; i++)
			{
				colors[colorIndex].rgb[i][0] = (sums[colorIndex].rgb[i][0] + (counts[colorIndex] >> 1)) / counts[colorIndex];
				colors[colorIndex].rgb[i][1] = (sums[colorIndex].rgb[i][1] + (counts[colorIndex] >> 1)) / counts[colorIndex];
				colors[colorIndex].rgb[i][2] = (sums[colorIndex].rgb[i][2] + (counts[colorIndex] >> 1)) / counts[colorIndex];
			}

			colorIndex++;
		}
		else
		{
			counts[colorIndex] = counts[--lightGridColorCount];
			memcpy(&sums[colorIndex], &sums[lightGridColorCount], sizeof(GfxLightGridColorSums));
			
			for (unsigned int i = 0; i < lightGridGlob->pointCount; i++)
			{
				if (lightGridGlob->points[i].entry.colorsIndex == lightGridColorCount)
					lightGridGlob->points[i].entry.colorsIndex = colorIndex;
			}
		}
	}

	delete[] counts;
	delete[] sums;
}
