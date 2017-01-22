#pragma once
#include <mutex>

#define VARIANCE_TRACKER _DEBUG

const static DWORD* g_LightmapCount = (DWORD*)0x16E99F58;

typedef void(*R_BuildFinalLightmaps_t)();
extern R_BuildFinalLightmaps_t o_R_BuildFinalLightmaps;

void hk_R_BuildFinalLightmaps();

BYTE __cdecl EncodeFloatInByte(float flt);

#if USE_LEGACY_HDR
void hk_R_StoreLightmapPixel();
void mfh_R_StoreLightmapPixel();
#else

#if VARIANCE_TRACKER
struct VarianceTracker
{
private:
	std::mutex mtx;

	double _min;
	double _max;
	double _total;

	unsigned int _count;

public:
	VarianceTracker();

	void Track(double v);

	double Min(void) const;
	double Max(void) const;
	double Total(void) const;
	double Average(void) const;
};

//
// Global variance trackers for the rewritten functions
//
extern VarianceTracker vt_GetInitialLightingHighlightDir;
extern VarianceTracker vt_GetColorsForHighlightDir_1;
extern VarianceTracker vt_GetColorsForHighlightDir_2;
extern VarianceTracker vt_GetLightingApproximationError;
extern VarianceTracker vt_GetGradientOfLightingErrorFunctionWithRespectToDir;
extern VarianceTracker vt_ImproveLightingApproximation_1;
extern VarianceTracker vt_ImproveLightingApproximation_2;

#ifndef VARIANCE_LOG
#define VARIANCE_LOG(TRACKER) Con_Printf("%s:\n\tMin: %f\n\tMax: %f\n\tAverage: %f\n", #TRACKER, TRACKER.Min(), TRACKER.Max(), TRACKER.Average());
#endif

#endif

void hk_StoreLightBytes();
void __cdecl StoreLightBytes(int lmapSet, int lmapRow, int pixelIndex, vec3* lighting, float* pFloats);
#endif