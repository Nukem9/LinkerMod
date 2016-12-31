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

void __cdecl GetInitialLightingHighlightDir_o(float* highlightDir, float* lighting)
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

void GetInitialLightingHighlightDir(vec3 *lighting, vec3 *out)
{
	float totals[256];

	//
	// Get the total R+G+B values for each sample and store it in 'totals'
	//
	if (g_basisDirectionsCount >= 4)
	{
		vec3* sampleColor = lighting;
		for (int i = 0; i < g_basisDirectionsCount; i++)
		{
			totals[i] = sampleColor->g + sampleColor->r + sampleColor->b;
			sampleColor++;
		}
	}

	vec3 v25;
	v25.x = 0;
	v25.y = 0;
	v25.z = 0;

	if (g_basisDirectionsCount >= 4)
	{
		vec3* dir = g_basisDirections;
		for (int i = 0; i < g_basisDirectionsCount; i++)
		{
			v25.z = dir->z * 0.5f + 0.5f;
			v25.y = totals[i] * v25.z + v25.y;
			v25.x = v25.z + v25.x;
			dir++;
		}
	}

	v25.y = v25.y / v25.x;

	out->x = 0.0f;
	out->y = 0.0f;
	out->z = 0.0f;

	if (g_basisDirectionsCount)
	{
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
	}

	Vec3Normalize(out);
}

void __cdecl GetColorsForHighlightDir_o(float* highlightDir, float* lighting, float* pel1, float* pel2)
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

void GetColorsForHighlightDir(vec3 *lighting, vec3 *highlightDir, vec3 *dstA, vec3 *dstB)
{
	vec3 baz;
	baz.x = 0;
	baz.y = 0;
	baz.z = 0;

	vec3 total_gamma__lighting;
	total_gamma__lighting.x = 0;
	total_gamma__lighting.y = 0;
	total_gamma__lighting.z = 0;

	vec3 total_dotpr__lighting;
	total_dotpr__lighting.x = 0;
	total_dotpr__lighting.y = 0;
	total_dotpr__lighting.z = 0;

	vec3 total = { 0, 0, 0 };
	for (int i = 0; i < g_basisDirectionsCount; i++)
	{
		total.r += lighting[i].r;
		total.g += lighting[i].g;
		total.b += lighting[i].b;
	}

	for (int i = 0; i < g_basisDirectionsCount; i++)
	{
		vec3* dir = &g_basisDirections[i];
		float gamma_ = dir->z * 0.5f + 0.5f; // I guess this is the power - like how much power the light reflected at a 90* angle has

		float dotpr_ = dir->x * highlightDir->x + dir->y * highlightDir->y + dir->z * highlightDir->z; // dotProduct(dir, highlightDir) // was named lensq_
		if (dotpr_ < 0.0f)
			dotpr_ = 0.0f;

		baz.x = dotpr_ * dotpr_ + baz.x;
		baz.y = gamma_ * gamma_ + baz.y;
		baz.z = dotpr_ * gamma_ + baz.z;

		/*
		diffuseLightColor = lightmapColor[0] ∗ dot(bumpBasis[0],normal) + lightmapColor[1] ∗ dot(bumpBasis[1],normal) + lightmapColor[2] ∗ dot(bumpBasis[2],normal) ....
		apparently highlightDir is really the normal and dotpr_ is dot(basisDir, normal)
		*/

		// these had lighting->r etc before but I think that was a bug
		total_gamma__lighting.x = lighting[i].r * gamma_ + total_gamma__lighting.x;
		total_gamma__lighting.y = lighting[i].g * gamma_ + total_gamma__lighting.y;
		total_gamma__lighting.z = lighting[i].b * gamma_ + total_gamma__lighting.z;

		// so this must be diffuse color
		total_dotpr__lighting.x = lighting[i].r * dotpr_ + total_dotpr__lighting.x;
		total_dotpr__lighting.y = lighting[i].g * dotpr_ + total_dotpr__lighting.y;
		total_dotpr__lighting.z = lighting[i].b * dotpr_ + total_dotpr__lighting.z;
	}

	float unk2 = baz.x * baz.y - baz.z * baz.z;
	if (unk2 == 0.0f) // this appears to be accurate at least
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
			unk2 = -baz.z;
			srcA.r = dstB->r * unk2 + total_gamma__lighting.x;
			srcA.g = dstB->g * unk2 + total_gamma__lighting.y;
			srcA.b = dstB->b * unk2 + total_gamma__lighting.z;

			// srcA = srcA / baz.y;
			unk2 = 1.0f / baz.y;
			srcA.r = srcA.r * unk2;
			srcA.g = srcA.g * unk2;
			srcA.b = srcA.b * unk2;
		}

		if (ClampColor(dstA, &srcA))
		{
			unk2 = -baz.z;
			srcB.r = dstA->r * unk2 + total_dotpr__lighting.x;
			srcB.g = dstA->g * unk2 + total_dotpr__lighting.y;
			srcB.b = dstA->b * unk2 + total_dotpr__lighting.z;

			unk2 = 1.0f / baz.x;
			srcB.r = srcB.r * unk2;
			srcB.g = srcB.g * unk2;
			srcB.b = srcB.b * unk2;

			ClampColor(dstB, &srcB);
		}
	}
}

float __cdecl GetLightingApproximationError_o(vec3 *lighting, vec3 *pel1, vec3 *pel2, vec3 *highlightDir)
{
	void* fn = (void*)0x004313B0;

	float oout = 0.0;
	_asm
	{
		push pel1
			push lighting
			mov edx, pel2
			mov eax, highlightDir
			call fn
			add esp, 8
			movd oout, xmm0
			xorps xmm0, xmm0
			xorps xmm1, xmm1

	}
	return oout;
}

double GetLightingApproximationError(vec3 *lighting, vec3 *pel1, vec3 *pel2, vec3 *highlightDir)
{
	double error = 0.0;

	for (int i = 0; i < g_basisDirectionsCount; i++)
	{
		vec3* basis = &g_basisDirections[i];

		float foo = basis->z * 0.5f + 0.5f;
		float dotp = Vec3Dot(basis, highlightDir);
		dotp = dotp < 0.0f ? 0.0f : dotp;

		float thing[3];
		thing[0] = pel1->r * foo + pel2->r * dotp;
		thing[1] = pel1->g * foo + pel2->g * dotp;
		thing[2] = pel1->b * foo + pel2->b * dotp;

		float bar[3];
		bar[0] = lighting[i].r - thing[0];
		bar[1] = lighting[i].g - thing[1];
		bar[2] = lighting[i].b - thing[2];

		double old = error;

		error += Vec3Dot(bar, bar);
	}

	return error;
}

//
// I don't have a name for this function right now
//
void sub_431DD9(vec3* pel1, vec3* pel2, vec3* dir, vec3* highlightDir, vec3* out)
{
	float unk = dir->z * 0.5f + 0.5f;
	float dotp = Vec3Dot(dir, highlightDir);
	if (dotp < 0.0)
		dotp = 0.0f;

	out->x = pel1->x * unk + pel2->x * dotp;
	out->y = pel1->y * unk + pel2->y * dotp;
	out->z = pel1->z * unk + pel2->z * dotp;
}

void __cdecl GetGradientOfLightingErrorFunctionWithRespectToDir_o(vec3 *lighting, vec3 *pel1, vec3 *pel2, vec3 *highlightDir, vec2 *gradient)
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
// Warning: DOES NOT WORK CORRECTLY
//
void GetGradientOfLightingErrorFunctionWithRespectToDir(vec3 *lighting, vec3 *pel1, vec3 *pel2, vec3 *highlightDir, vec2 *gradient)
{
	float total[2];
	total[0] = 0.0f;
	total[1] = 0.0f;

	for (int i = 0; i < g_basisDirectionsCount; i++)
	{
		vec3* basis = &g_basisDirections[i];

		vec3 out;
		sub_431DD9(pel1, pel2, basis, highlightDir, &out);

		float yaa = Vec3Dot(basis, highlightDir);

		float tmp[2];
		tmp[0] = highlightDir->x * -yaa + basis->x;
		tmp[1] = highlightDir->y * -yaa + basis->y;

		float bar[3];
		bar[0] = lighting[i].r - out.x;
		bar[1] = lighting[i].g - out.y;
		bar[2] = lighting[i].b - out.z;

		float dotp = Vec3Dot(pel2, (vec3*)bar);
		total[0] += tmp[0] * dotp;
		total[1] += tmp[1] * dotp;
	}

	float unk = highlightDir->z + highlightDir->z;
	gradient->x = total[0] * unk;
	gradient->y = total[1] * unk;
}

void __cdecl ImproveLightingApproximation_o(float* highlightDir, float* lighting, float* pel1, float* pel2)
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
// Warning: DOES NOT WORK CORRECTLY
//
void __cdecl ImproveLightingApproximation(vec3 *lighting, vec3* pel1, vec3* pel2, vec3 *highlightDir)
{
	float err = (float)GetLightingApproximationError(lighting, pel1, pel2, highlightDir);
	if (err == 0.0f)
	{
		return;
	}

	BYTE b_dir[2];
	b_dir[0] = EncodeFloatInByte(highlightDir->x / highlightDir->z * 0.25f + 0.5f);
	b_dir[1] = EncodeFloatInByte(highlightDir->y / highlightDir->z * 0.25f + 0.5f);

	int magic = 4;

	while (1)
	{
		vec2 gradient;
		GetGradientOfLightingErrorFunctionWithRespectToDir(lighting, pel1, pel2, highlightDir, &gradient);

		vec2 abs_grad;
		abs_grad.x = fabs(gradient.x);
		abs_grad.y = fabs(gradient.y);

		if (abs_grad.x - abs_grad.y < 0.0f)
			abs_grad.x = abs_grad.y;

		vec3 dir;
		int baz[2]; // the new b_dir

		vec3 new_pel1;
		vec3 new_pel2;

		float err2 = 0.0f;

		while (1)
		{
			float oob = magic / abs_grad.x;

			baz[0] = ClampByte(b_dir[0] + (int)(gradient.x * oob));
			baz[1] = ClampByte(b_dir[1] + (int)(gradient.y * oob));


			dir.x = baz[0] / 63.75f - 2.0f; // possibly 255 / max_magic - max_magic / 2 ?
			dir.y = baz[1] / 63.75f - 2.0f; // possibly 255 / max_magic
			dir.z = 1.0f;

			Vec3Normalize(&dir);

			GetColorsForHighlightDir(lighting, &dir, &new_pel1, &new_pel2);
			err2 = (float)GetLightingApproximationError(lighting, &new_pel1, &new_pel2, &dir);
			if (err > err2)
				break;

			magic /= 2;
			if (!magic)
				return;
		}

		pel1->r = new_pel1.r;
		pel1->g = new_pel1.g;
		pel1->b = new_pel1.b;

		pel2->r = new_pel2.r;
		pel2->g = new_pel2.g;
		pel2->b = new_pel2.b;

		b_dir[0] = baz[0];
		b_dir[1] = baz[1];

		highlightDir->x = dir.x;
		highlightDir->y = dir.y;
		highlightDir->z = dir.z;

		err = err2;
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
*/
void __cdecl StoreLightBytes(int lmapSet, int lmapRow, int pixelIndex, vec3* lighting, float* pFloats)
{
	int subOffset = 0x600 * lmapSet + lmapRow;

	vec3 highlightDir;
	GetInitialLightingHighlightDir(lighting, &highlightDir);

	vec3 pel1;
	vec3 pel2;
	GetColorsForHighlightDir(lighting, &highlightDir, &pel1, &pel2);

	//ImproveLightingApproximation((vec3*)highlightDir, (vec3*)lighting, (vec3*)pel1, (vec3*)pel2);
	ImproveLightingApproximation_o((float*)&highlightDir, (float*)lighting, (float*)&pel1, (float*)&pel2);

	BYTE* lightBytes = (BYTE*)0x00471590;
	WAW_LMAP_PEL* pel = (WAW_LMAP_PEL*)&lightBytes[4 * (pixelIndex + (subOffset << 9))];

	pel->B = EncodeFloatInByte(pel1.b);
	pel->G = EncodeFloatInByte(pel1.g);
	pel->R = EncodeFloatInByte(pel1.r);
	pel->A = EncodeFloatInByte(highlightDir.x / highlightDir.z * 0.25f + 0.5f);

	pel += 0x40000;

	pel->B = EncodeFloatInByte(pel2.b);
	pel->G = EncodeFloatInByte(pel2.g);
	pel->R = EncodeFloatInByte(pel2.r);
	pel->A = EncodeFloatInByte(highlightDir.y/ highlightDir.z * 0.25f + 0.5f);

	if (g_HDR)
	{
		vec4* out1 = &LightmapBytes_HDR[0x40000 * lmapSet + 512 * lmapRow + pixelIndex];
		out1->r = pel1.r;
		out1->g = pel1.g;
		out1->b = pel1.b;
		out1->a = highlightDir.x / highlightDir.z * 0.25f + 0.5f;

		vec4* out2 = &Lightmap2Bytes_HDR[0x40000 * lmapSet + 512 * lmapRow + pixelIndex];
		out2->r = pel2.r;
		out2->g = pel2.g;
		out2->b = pel2.b;
		out2->a = highlightDir.y / highlightDir.z * 0.25f + 0.5f;
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
