#pragma once

static int& g_basisDirectionsCount = *(int*)0x16E99F70;
static vec3*& g_basisDirections = *(vec3**)0x16E99F74;

static float& g_Gamma = *(float*)0x153C907C;

double __cdecl GammaCorrect(float color);
void __cdecl GammaCorrectColor(float *rgb);
void __cdecl GammaCorrectColor(vec3* rgb);

bool ClampColor(vec3 *dst, vec3 *src, float max = 1.0f);


void EncodeNormalToFloats(vec3* normal, vec2* out);
void EncodeNormalToBytes(vec3* normal, BYTE(&out)[2]);

/*
	Generally the incoming packed values are really:
	packed_xz = normal.x / normal.z * 0.25 + 0.5
	packed_yz = normal.y / normal.z * 0.25 + 0.5
*/
void DecodeNormalFromBytes(int packed_xz, int packed_yz, vec3* out);
void DecodeNormalFromFloats(vec2* packed, vec3* out);
