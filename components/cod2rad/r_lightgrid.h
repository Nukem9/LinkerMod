#pragma once

#define LIGHTGRID_MAX_COLORCOUNT 0xFFFF

const static DWORD* g_lightgridSampleCount = (DWORD*)0x153C91D0;
const static DWORD* g_diskLightgridSampleCount = (DWORD*)0x153C91E0;

struct GfxLightGridColors
{
	union
	{
		BYTE rgb[56][3];
		BYTE all[56 * 3];
	};
};
STATIC_ASSERT_SIZE(GfxLightGridColors, 56 * 3);

struct GridColorsCluster
{
	unsigned int first;		// 0x00
	unsigned int count;		// 0x04
	unsigned int unknown1;	// 0x08
	float unknown2;			// 0x0C
	float unknown3;			// 0x10
};
STATIC_ASSERT_SIZE(GridColorsCluster, 0x14);

struct GfxLightGridEntry
{
	unsigned __int16 colorsIndex;
	char primaryLightIndex;
	char needsTrace;
};
STATIC_ASSERT_SIZE(GfxLightGridEntry, 0x04);

struct GridSamplePoint
{
	WORD pos[3];	// 0x00
	WORD unknown4;	// 0x06 ???
	//WORD unknown5;	// 0x08 ColorsIndex?
	//BYTE unknown6;	// 0x0A	PrimaryLightIndex?
	//BYTE unknown7;	// 0x0C NeedsTrace?
	GfxLightGridEntry entry;
};
STATIC_ASSERT_SIZE(GridSamplePoint, 0xC);

struct LightGridGlob
{
	unsigned int pointCount;		// 0x00 0x153C91D0
	unsigned int maxPoints;			// 0x04 0x153C91D4
	GridSamplePoint* points;			// 0x08 0x153C91D8
	unsigned int dword_153C91DC;	// 0x0C 0x153C91DC
	unsigned int clusterCount;		// 0x10 0x153C91E0
	GridColorsCluster *clusters;	// 0x14 0x153C91E4
	unsigned int *mapping;			// 0x18 0x153C91E8
	unsigned int dword_153C91EC;	// 0x1C 0x153C91EC
	unsigned int dword_153C91F0;	// 0x20 0x153C91F0
	float flt_153C91F4;				// 0x24 0x153C91F4
	float flt_153C91F8;				// 0x28 0x153C91F8
	float flt_153C91FC;				// 0x2C 0x153C91FC
	float flt_153C9200;				// 0x30 0x153C9200
	unsigned int dword_153C9204;	// 0x34 0x153C9204
	// ...
};

static LightGridGlob *lightGridGlob = (LightGridGlob *)0x153C91D0;

static unsigned int& lightGridColorCount = *(unsigned int *)0x112BAAB4;

static bool& options_ImproveLights = *(bool *)0x153C9005;
static float& options_clusterThreshold = *(float*)0x153C902C;

typedef WORD DiskSampleColorHDR[56][3];
typedef BYTE DiskSampleColor[56][3];

static QuantumFunc_t GuessLightGridColors = (QuantumFunc_t)0x004342F0;

#if USE_LEGACY_HDR
typedef float SampleColorHDR[56][3];
typedef WORD DiskSampleColorHDR[56][3];
typedef BYTE DiskSampleColor[56][3];

typedef void(*R_Init_Lightgrid_t)();
extern R_Init_Lightgrid_t o_R_Init_Lightgrid;

typedef void(*R_Store_QuantizedLightGridSample_t)();
extern R_Store_QuantizedLightGridSample_t o_R_Store_QuantizedLightGridSample;

void mfh_R_Init_Lightgrid();
void mfh_R_Store_LightgridSample();
void mfh_R_Alloc_DiskLightGridColors();
void hk_R_Store_QuantizedLightGridSample();
#else

void mfh_R_Init_Lightgrid();

typedef void(__cdecl* SwapClusters_t)(int fromIndex, int toIndex);
static SwapClusters_t SwapClusters = (SwapClusters_t)0x00432EF0;

void __cdecl StoreLightingForDir(vec3* lighting, BYTE* dst);
void __cdecl ClusterLightGridValues(int ThreadCount);
void __cdecl ImproveLightGridValues(int threadCount);
#endif
