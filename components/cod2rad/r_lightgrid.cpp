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

GfxLightGridColorsHDR* g_lightGridColorsHDR = nullptr; // The HDR version of lightGridGlob->colors
GfxLightGridColorsHDR disk_lightGridColorsHDR[0xFFFF];

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
void __cdecl StoreLightingForDir(vec3* lighting, GfxLightGridColors* dst)
{
	const int colorIndex = dst - (GfxLightGridColors*)lightGridGlob->colors;

	for (int i = 0; i < 56; i++)
	{
		GammaCorrectColor(&lighting[i]);
	}

	AdjustLightingContrast(56, -1, lighting);

	for (int sampleIndex = 0; sampleIndex < 56; sampleIndex++)
	{
		vec3 v = lighting[sampleIndex] * 0.5;
		dst->rgb[sampleIndex][0] = EncodeFloatInByte(v.r);
		dst->rgb[sampleIndex][1] = EncodeFloatInByte(v.g);
		dst->rgb[sampleIndex][2] = EncodeFloatInByte(v.b);

		g_lightGridColorsHDR[colorIndex].rgb[sampleIndex][0] = EncodeFloatInWord(v.r);
		g_lightGridColorsHDR[colorIndex].rgb[sampleIndex][1] = EncodeFloatInWord(v.g);
		g_lightGridColorsHDR[colorIndex].rgb[sampleIndex][2] = EncodeFloatInWord(v.b);
	}
}

void CalculateClusterMean_o(GridColorsCluster *cluster, float *means)
{
	static DWORD dwCall = 0x004331A0;

	__asm
	{
		mov ecx, cluster
			mov eax, means
			call dwCall
	}
}

void CalculateClusterMean(GridColorsCluster *cluster, float *means)
{
	float sums[168];
	memset(sums, 0, sizeof(float) * 168);

	for (unsigned int clusterIndex = 0; clusterIndex < cluster->count; clusterIndex++)
	{
		GfxLightGridColors* colors = &lightGridGlob->colors[lightGridGlob->mapping[cluster->first + clusterIndex]];
		for (int i = 0; i < 168; i++)
		{
			sums[i] += colors->all[i];
		}
	}

	for (int i = 0; i < 168; i++)
	{
		means[i] = sums[i] / cluster->count;
	}
}

void CalculateClusterMeanHDR(GridColorsCluster *cluster, float *means)
{
	float sums[168];
	memset(sums, 0, sizeof(float) * 168);

	for (unsigned int clusterIndex = 0; clusterIndex < cluster->count; clusterIndex++)
	{
		GfxLightGridColorsHDR* colors = &g_lightGridColorsHDR[lightGridGlob->mapping[cluster->first + clusterIndex]];
		for (int i = 0; i < 168; i++)
		{
			sums[i] += colors->all[i];
		}
	}

	for (int i = 0; i < 168; i++)
	{
		means[i] = sums[i] / cluster->count;
	}
}

void CalculateClusterMeanAndVariance_o(GridColorsCluster *cluster)
{
	ASSERT(cluster);
	ASSERT(cluster->count);

	static DWORD dwCall = 0x00434320;

	__asm
	{
		mov edi, cluster
		call dwCall
	}
}

void CalculateClusterMeanAndVariance(GridColorsCluster *cluster)
{
	ASSERT(cluster);
	ASSERT(cluster->count);

	float means[168];
	CalculateClusterMean(cluster, means);

	float totals[168];
	memset(totals, 0, sizeof(float) * 168);

	int firstIndex = lightGridGlob->mapping[cluster->first];
	for (unsigned int clusterIndex = 0; clusterIndex < cluster->count; clusterIndex++)
	{
		for (int i = 0; i < 168; i++)
		{
			double v = lightGridGlob->colors[firstIndex + clusterIndex].all[i] - means[i];
			totals[i] = (float)(v * v + totals[i]);
		}
	}

	cluster->unknown3 = -FLT_MAX;
	for (int i = 0; i < 168; i++)
	{
		float unk = sqrt(totals[i] / cluster->count);
		if (cluster->unknown3 < unk)
		{
			cluster->unknown3 = unk;
			cluster->unknown1 = i;
			cluster->unknown2 = means[i];
		}
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

void SplitCluster(GridColorsCluster *cluster)
{
	ASSERT(cluster);
	ASSERT(cluster->count >= 2);

	static DWORD dwCall = 0x00434B50;

	__asm
	{
		mov esi, cluster
		call dwCall
	}
}

int GetClusterDefaultScore(GridColorsCluster *cluster)
{
	static DWORD dwCall = 0x00432E90;

	__asm
	{
		mov ecx, cluster
		call dwCall
	}
}

void SetLightGridColorsForCluster(GridColorsCluster *cluster, GfxLightGridColors *colors, GfxLightGridColorsHDR* colorsHDR=NULL)
{
	float means[168];
	CalculateClusterMeanHDR(cluster, means);

	for (int i = 0; i < 168; i++)
	{
		colors->all[i] = (BYTE)means[i];
		if (colorsHDR)
			colorsHDR->all[i] = (short)means[i];
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

void __cdecl ClusterLightGridValues(int threadCount)
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
		for (unsigned int colorIndex = 0; colorIndex < lightGridGlob->clusterCount; colorIndex++)
		{
			GridColorsCluster *cluster = &lightGridGlob->clusters[colorIndex];
			for (unsigned int firstIndex = cluster->first; firstIndex < (cluster->first + cluster->count); firstIndex++)
			{
				lightGridGlob->points[lightGridGlob->mapping[firstIndex]].entry.colorsIndex = colorIndex;
			}
			
			SetLightGridColorsForCluster(cluster, &disk_lightGridColors[colorIndex], &disk_lightGridColorsHDR[colorIndex]);
			
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
		ImproveLightGridValues(threadCount);
	
	delete[] lightGridGlob->clusters;
	delete[] lightGridGlob->mapping;
	lightGridGlob->clusters = nullptr;
	lightGridGlob->mapping = nullptr;
	
	--lightGridGlob->pointCount;
}

bool CompareLightGridColors(GfxLightGridColors* a, GfxLightGridColors* b, unsigned int* out)
{
	unsigned int totalDifference = 0;

	for (int i = 0; i < 56; i++)
	{
		for (int c = 0; c < 3; c++)
		{
			totalDifference += a->rgb[i][c] - a->rgb[i][c];
			if (totalDifference >= *out)
				return false;
		}
	}

	*out = totalDifference;
	return true;
}

unsigned short AssignLightGridColors(unsigned short colorIndex, GfxLightGridColors* colors)
{
	unsigned int unk = 0x7FFFFFFF;
	CompareLightGridColors(&lightGridGlob->colors[colorIndex], colors, &unk);
	if (!unk)
	{
		return colorIndex;
	}

	for (unsigned int i = 0; i < lightGridGlob->clusterCount; i++)
	{
		if (CompareLightGridColors(&lightGridGlob->colors[i], colors, &unk))
		{
			if (!unk)
			{
				colorIndex = i;
				break;
			}
		}
	}

	return colorIndex;
}

void __cdecl GuessLightGridColors(int index, int unk)
{
	GfxLightGridEntry *entry = &lightGridGlob->points[index].entry;
	entry->colorsIndex = AssignLightGridColors(entry->colorsIndex, &lightGridGlob->colors[index]);
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
	EndProgress();

	for (unsigned int pointIndex = 0; pointIndex < lightGridGlob->pointCount; pointIndex++)
	{
		counts[lightGridGlob->points[pointIndex].entry.colorsIndex]++;
		GfxLightGridColors* colors = (GfxLightGridColors*)lightGridGlob->colors;
		for (int i = 0; i < 56; i++)
		{
			sums[lightGridGlob->points[pointIndex].entry.colorsIndex].rgb[i][0] += colors[pointIndex].rgb[i][0];
			sums[lightGridGlob->points[pointIndex].entry.colorsIndex].rgb[i][1] += colors[pointIndex].rgb[i][1];
			sums[lightGridGlob->points[pointIndex].entry.colorsIndex].rgb[i][2] += colors[pointIndex].rgb[i][2];
		}
	}
	
	for (unsigned int colorIndex = 0; colorIndex < lightGridColorCount; )
	{
		if (counts[colorIndex])
		{
			for (int i = 0; i < 56; i++)
			{
				disk_lightGridColors[colorIndex].rgb[i][0] = (sums[colorIndex].rgb[i][0] + (counts[colorIndex] >> 1)) / counts[colorIndex];
				disk_lightGridColors[colorIndex].rgb[i][1] = (sums[colorIndex].rgb[i][1] + (counts[colorIndex] >> 1)) / counts[colorIndex];
				disk_lightGridColors[colorIndex].rgb[i][2] = (sums[colorIndex].rgb[i][2] + (counts[colorIndex] >> 1)) / counts[colorIndex];
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
