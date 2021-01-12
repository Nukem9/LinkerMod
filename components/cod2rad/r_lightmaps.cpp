#include "stdafx.h"

#define LOG_VEC_EQ(A,B) if (A != B) { printf("(%f) %f %f %f == %f %f %f\n", Vec3Variance(&A, &B), A.x, A.y, A.z, B.x, B.y, B.z); }

#if VARIANCE_TRACKER
VarianceTracker vt_GetInitialLightingHighlightDir;
VarianceTracker vt_GetColorsForHighlightDir_1;
VarianceTracker vt_GetColorsForHighlightDir_2;
VarianceTracker vt_GetLightingApproximationError;
VarianceTracker vt_GetGradientOfLightingErrorFunctionWithRespectToDir;
VarianceTracker vt_ImproveLightingApproximation_1;
VarianceTracker vt_ImproveLightingApproximation_2;

VarianceTracker::VarianceTracker() : _min(FLT_MAX), _max(DBL_MIN), _total(0.0), _count(0)
{
}

void VarianceTracker::Track(double v)
{
	mtx.lock();
	if (v < _min)
		_min = v;

	if (v > _max)
		_max = v;

	_total += v;
	_count++;
	mtx.unlock();
}

double VarianceTracker::Min(void) const { return _min; }
double VarianceTracker::Max(void) const { return _max; }
double VarianceTracker::Total(void) const { return _total; }
double VarianceTracker::Average(void) const { return _total / _count; }

#endif

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

void __cdecl GetInitialLightingHighlightDir_o(vec3* lighting, vec3* highlightDir)
{
	_asm
	{
		pushad
		push lighting
		mov eax, highlightDir
		mov ebx, 0x00431980
		call ebx
		add esp, 4
		popad
	}
}

//
// Verified
// Variance: 0.000000 0.000014
//
void GetInitialLightingHighlightDir(vec3 *lighting, vec3 *out)
{
	float totals[256];

	//
	// Get the total R+G+B values for each sample and store it in 'totals'
	//
	for (int i = 0; i < g_basisDirectionsCount; i++)
	{
		totals[i] = lighting[i].r + lighting[i].g + lighting[i].b;
	}

	vec3 v25;
	v25.x = 0;
	v25.y = 0;
	v25.z = 0;

	for (int i = 0; i < g_basisDirectionsCount; i++)
	{
		v25.z = g_basisDirections[i].z * 0.5f + 0.5f;
		v25.y = totals[i] * v25.z + v25.y;
		v25.x = v25.z + v25.x;
	}

	v25.y = v25.y / v25.x;

	out->x = 0.0f;
	out->y = 0.0f;
	out->z = 0.0f;

	for (int i = 0; i < g_basisDirectionsCount; i++)
	{
		vec3* dir = &g_basisDirections[i];
		v25.z = dir->z * 0.5f + 0.5f;
		v25.z = totals[i] - v25.z * v25.y;
		totals[i] = v25.z; 

		out->x = dir->x * v25.z + out->x;
		out->y = dir->y * v25.z + out->y;
		out->z = dir->z * v25.z + out->z;
	}

	Vec3Normalize(out);
}

void __cdecl GetColorsForHighlightDir_o(vec3* lighting, vec3* highlightDir, vec3* pel1, vec3* pel2)
{
	_asm
	{
		pushad
		push pel2
		push pel1
		mov ecx, lighting
		mov eax, highlightDir
		mov ebx, 0x00430EF0
		call ebx
		add esp, 8
		popad
	}
}

//
// Verified
// Variance: 0.000000 0.000393 (pel1)
// Variance: 0.000000 0.000372 (pel2)
//
void GetColorsForHighlightDir(vec3 *lighting, vec3 *highlightDir, vec3 *dstA, vec3 *dstB)
{
	vec3 total(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < g_basisDirectionsCount; i++)
	{
		total += lighting[i];
	}

	vec3 baz(0.0f, 0.0f, 0.0f);
	vec3 total_gamma__lighting(0.0f, 0.0f, 0.0f);
	vec3 total_dotpr__lighting(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < g_basisDirectionsCount; i++)
	{
		vec3* dir = &g_basisDirections[i];
		float gamma_ = dir->z * 0.5f + 0.5f;

		float dotpr_ = Vec3Dot(dir, highlightDir);
		if (dotpr_ < 0.0f)
			dotpr_ = 0.0f;

		baz.x = dotpr_ * dotpr_ + baz.x;
		baz.y = gamma_ * gamma_ + baz.y;
		baz.z = dotpr_ * gamma_ + baz.z;

		total_gamma__lighting += lighting[i] * gamma_;
		total_dotpr__lighting += lighting[i] * dotpr_;
	}

	float unk2 = baz.x * baz.y - baz.z * baz.z;
	if (unk2 == 0.0f)
	{
		dstA->r = 0.0;
		dstA->g = 0.0;
		dstA->b = 0.0;
		dstB->r = 0.0;
		dstB->g = 0.0;
		dstB->b = 0.0;
	}
	else
	{
		vec3 srcA;
		vec3 srcB;

		unk2 = 1.0f / unk2;
		srcA.r = (total_gamma__lighting.x * baz.x - total_dotpr__lighting.x * baz.z) * unk2;
		srcB.r = (total_dotpr__lighting.x * baz.y - total_gamma__lighting.x * baz.z) * unk2;
		srcA.g = (total_gamma__lighting.y * baz.x - total_dotpr__lighting.y * baz.z) * unk2;
		srcB.g = (total_dotpr__lighting.y * baz.y - total_gamma__lighting.y * baz.z) * unk2;
		srcA.b = (total_gamma__lighting.z * baz.x - total_dotpr__lighting.z * baz.z) * unk2;
		srcB.b = (total_dotpr__lighting.z * baz.y - total_gamma__lighting.z * baz.z) * unk2;

		if (ClampColor(dstB, &srcB))
		{
			srcA = *dstB * -baz.z + total_gamma__lighting;
			srcA /= baz.y;
		}

		if (ClampColor(dstA, &srcA))
		{
			srcB = *dstA * -baz.z + total_dotpr__lighting;
			srcB /= baz.x;

			ClampColor(dstB, &srcB);
		}
	}
}

//
// Regenerate the lighting sample from dir, highlightDir, and the two pels
//
void GetLightingSampleForDirFromLightmap(vec3* dir, vec3* highlightDir, vec3* pel_amb, vec3* pel_dir, vec3* out)
{
	// It would appear that half lambert is used for the ambient lighting
	float weight_amb = dir->z * 0.5f + 0.5f; // weight_amb = Dot(dir, vec3(0, 0, 1)) * 0.5 + 0.5;
	float weight_dir = Vec3Dot(dir, highlightDir);

	if (weight_dir < 0.0)
		weight_dir = 0.0f;

	*out = (*pel_amb) * weight_amb + (*pel_dir) * weight_dir;
}

float GetLightingApproximationError_o(vec3 *lighting, vec3 *highlightDir, vec3 *pel1, vec3 *pel2)
{
	static DWORD dwCall = 0x004313B0;

	DWORD outFloat;

	_asm
	{
		push pel1
		push lighting
		mov edx, pel2
		mov eax, highlightDir
		call [dwCall]
		add esp, 8
		movd [outFloat], xmm0
		xorps xmm0, xmm0
		xorps xmm1, xmm1
	}

	return *(float *)&outFloat;
}

/*
	Get the total value of error^2 for each lighting sample and the current lightmap pixels
	Variance: 0.000000 0.000001
*/
double GetLightingApproximationError(vec3 *lighting, vec3 *highlightDir, vec3 *pel_amb, vec3 *pel_dir)
{
	double error = 0.0;

	for (int i = 0; i < g_basisDirectionsCount; i++)
	{
		vec3 sample;
		GetLightingSampleForDirFromLightmap(&g_basisDirections[i], highlightDir, pel_amb, pel_dir, &sample);

		vec3 dif = lighting[i] - sample;
		error += Vec3Dot(&dif, &dif);
	}

	return error;
}

void __cdecl GetGradientOfLightingErrorFunctionWithRespectToDir_o(vec3 *lighting, vec3 *highlightDir, vec3 *pel1, vec3 *pel2, vec2 *gradient)
{
	void* fn = (void*)0x00431D50;
	_asm
	{
		push gradient
		push highlightDir
		push pel1
		mov ecx, lighting
		mov eax, pel2
		call fn
		add esp, 12
	}
}

//
// Verified
// Variance: 0.000000 0.000002
//
void GetGradientOfLightingErrorFunctionWithRespectToDir(vec3 *lighting, vec3 *highlightDir, vec3 *pel1, vec3 *pel2, vec2 *gradient)
{
	vec2 total(0.0f, 0.0f);

	for (int i = 0; i < g_basisDirectionsCount; i++)
	{
		vec3* basis = &g_basisDirections[i];

		vec3 sample;
		GetLightingSampleForDirFromLightmap(basis, highlightDir, pel1, pel2, &sample);

		float yaa = Vec3Dot(basis, highlightDir);

		vec2 tmp;
		tmp.x = highlightDir->x * -yaa + basis->x;
		tmp.y = highlightDir->y * -yaa + basis->y;

		vec3 dif = lighting[i] - sample;
		
		float dotp = Vec3Dot(pel2, &dif);
		total += tmp * dotp;
	}

	*gradient = total * (highlightDir->z + highlightDir->z);
}

void __cdecl ImproveLightingApproximation_o(vec3* lighting, vec3* highlightDir, vec3* pel1, vec3* pel2)
{
	_asm
	{
		pushad
		push pel2
		push pel1
		push lighting
		mov edi, highlightDir
		mov ebx, 0x004323E0
		call ebx
		add esp, 12
		popad
	}
}

//
// Verified: Bad
// Variance: 0.000000 0.813436 (pel1)
// Variance: 0.000000 0.590278 (pel2)
//
void ImproveLightingApproximation(vec3* lighting, vec3 *highlightDir, vec3* pel_amb, vec3* pel_dir)
{
	double curError = 0.0;
	double error = GetLightingApproximationError(lighting, highlightDir, pel_amb, pel_dir);

#if VARIANCE_TRACKER
	double err2 = GetLightingApproximationError_o(lighting, highlightDir, pel_amb, pel_dir);
	vt_GetLightingApproximationError.Track(abs(error - err2));
#endif

	// Don't fix what isn't broken
	if (error == 0.0)
		return;

	BYTE initialByteDir[2];
	BYTE updatedByteDir[2];

	EncodeNormalToBytes(highlightDir, initialByteDir);

	// the formula for weight is 2^(2-x) where x is the iteration number
	float weight = 4.0;
	for(int iterations = 16;;) // the original number of iterations was 4
	{
		vec2 gradient;
		GetGradientOfLightingErrorFunctionWithRespectToDir(lighting, highlightDir, pel_amb, pel_dir, &gradient);

#if VARIANCE_TRACKER
		vec2 gradient2;
		GetGradientOfLightingErrorFunctionWithRespectToDir_o(lighting, highlightDir, pel_amb, pel_dir, &gradient2);
		vt_GetGradientOfLightingErrorFunctionWithRespectToDir.Track(Vec2Variance(&gradient, &gradient2));
#endif

		vec2 absoluteGradient;
		absoluteGradient.x = fabs(gradient.x);
		absoluteGradient.y = fabs(gradient.y);

		float tmp = absoluteGradient.x;
		if (absoluteGradient.x - absoluteGradient.y < 0.0f)
			tmp = absoluteGradient.y;

		vec3 dir;
		vec3 new_pel_amb;
		vec3 new_pel_dir;

		//
		// Adjust using the gradient, keep adjustment if the new error is lower than the old one, otherwise try again
		//
		while (true)
		{
			float scalar = weight / tmp;

			updatedByteDir[0] = ClampByte(initialByteDir[0] + (int)(gradient.x * scalar));
			updatedByteDir[1] = ClampByte(initialByteDir[1] + (int)(gradient.y * scalar));

			DecodeNormalFromBytes(updatedByteDir[0], updatedByteDir[1], &dir);

			GetColorsForHighlightDir(lighting, &dir, &new_pel_amb, &new_pel_dir);
#if VARIANCE_TRACKER
			vec3 new_pel3;
			vec3 new_pel4;
			GetColorsForHighlightDir_o(lighting, &dir, &new_pel3, &new_pel4);
			vt_GetColorsForHighlightDir_1.Track(Vec3Variance(&new_pel_amb, &new_pel3));
			vt_GetColorsForHighlightDir_2.Track(Vec3Variance(&new_pel_dir, &new_pel4));
#endif
			curError = GetLightingApproximationError(lighting, &dir, &new_pel_amb, &new_pel_dir);
#if VARIANCE_TRACKER
			err2 = GetLightingApproximationError_o(lighting, &dir, &new_pel_amb, &new_pel_dir);
			vt_GetLightingApproximationError.Track(abs(curError - err2));
#endif

			//
			// If the adjusted approximation has a smaller error value - use the approximation
			//
			if (curError < error)
				break;

			weight /= 2.0f;

			// Abort if we were unable to find a better approximation within a reasonable amount of iterations
			if (!--iterations)
			{
				return;
			}
		}

		*pel_amb = std::move(new_pel_amb);
		*pel_dir = std::move(new_pel_dir);

		initialByteDir[0] = updatedByteDir[0];
		initialByteDir[1] = updatedByteDir[1];

		*highlightDir = std::move(dir);

		error = curError;
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

/*
	lighting - a pointer to an array of [g_basisDirectionsCount] RGB vec3 values for the given pixel - these correspond to each basis direction in g_basisDirections for that point
	pFloats - a pointer to the primary light values (1 float per pixel) that are used for baked shadowmaps

	pel1 - amb
	pel2 - dir
	pFloats - sm
*/
void __cdecl StoreLightBytes(int lmapSet, int lmapRow, int pixelIndex, vec3* lighting, float* pFloats)
{
	int subOffset = 0x600 * lmapSet + lmapRow;

	vec3 highlightDir;
	GetInitialLightingHighlightDir(lighting, &highlightDir);

#if VARIANCE_TRACKER
	vec3 highlightDir2;
	GetInitialLightingHighlightDir_o(lighting, &highlightDir2);

	vt_GetInitialLightingHighlightDir.Track(Vec3Variance(&highlightDir, &highlightDir2));
#endif

	vec3 pel1;
	vec3 pel2;
	GetColorsForHighlightDir(lighting, &highlightDir, &pel1, &pel2);

#if VARIANCE_TRACKER
	vec3 pel3;
	vec3 pel4;
	GetColorsForHighlightDir_o(lighting, &highlightDir, &pel3, &pel4);

	vt_GetColorsForHighlightDir_1.Track(Vec3Variance(&pel1, &pel3));
	vt_GetColorsForHighlightDir_2.Track(Vec3Variance(&pel2, &pel4));
#endif

	ImproveLightingApproximation(lighting, &highlightDir, &pel1, &pel2);
#if VARIANCE_TRACKER
	// Ensure that the starting values are the same
	pel3 = pel1;
	pel4 = pel2;

	ImproveLightingApproximation_o(lighting, &highlightDir, &pel3, &pel4);
	vt_ImproveLightingApproximation_1.Track(Vec3Variance(&pel1, &pel3));
	vt_ImproveLightingApproximation_2.Track(Vec3Variance(&pel2, &pel4));
#endif

	BYTE packed_normal[2];
	EncodeNormalToBytes(&highlightDir, packed_normal);

	BYTE* lightBytes = (BYTE*)0x00471590;
	WAW_LMAP_PEL* pel = (WAW_LMAP_PEL*)&lightBytes[4 * (pixelIndex + (subOffset << 9))];

	pel->B = EncodeFloatInByte(pel1.b);
	pel->G = EncodeFloatInByte(pel1.g);
	pel->R = EncodeFloatInByte(pel1.r);
	pel->A = packed_normal[0];

	pel += 0x40000;

	pel->B = EncodeFloatInByte(pel2.b);
	pel->G = EncodeFloatInByte(pel2.g);
	pel->R = EncodeFloatInByte(pel2.r);
	pel->A = packed_normal[0];

	if (g_HDR)
	{
		vec2 n;
		EncodeNormalToFloats(&highlightDir, &n);

		vec4* out1 = &LightmapBytes_HDR[0x40000 * lmapSet + 512 * lmapRow + pixelIndex];
		*out1 = vec4(pel1, n.x);

		vec4* out2 = &Lightmap2Bytes_HDR[0x40000 * lmapSet + 512 * lmapRow + pixelIndex];
		*out2 = vec4(pel2, n.y);
	}

	//
	// For each lightmap pixel copy a block of 4 pixels for the shadowmap
	// since the colored images are 512x512 and the shadowmap is 1024x1024 this is necessary
	//
	BYTE* lightBytes_PrimaryImage = (BYTE*)0x00671590;
	BYTE* sm_pel = &lightBytes_PrimaryImage[2 * (pixelIndex + (subOffset << 10))];

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			sm_pel[1024*i + j] = EncodeFloatInByte(pFloats[2*i + j]);
		}
	}
}

#endif
