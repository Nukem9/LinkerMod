#pragma once

#define BIT_INDEX_32(bits)	((bits) >> 5)
#define BIT_MASK_32(bits)	(1 << ((bits) & 31))

#define IS_NAN(x)			((*(unsigned int *)(&x) & 0x7F800000) == 0x7F800000)

float Vec3DistanceSq(const float *p1, const float *p2);
float Vec3Dot(float *p1, float *p2);
float Vec3Length(float *p1, float *p2);
void Vec3Normalize(float *v);
float flrand(float min, float max);

void __cdecl AxisCopy(const float(*in)[3], float(*out)[3]);

// /universal/com_math.h:593
static int CountBitsEnabled(unsigned int num)
{
	unsigned int numa = (((((num >> 1) & 0x55555555) + (num & 0x55555555)) >> 2) & 0x33333333)
		+ ((((num >> 1) & 0x55555555) + (num & 0x55555555)) & 0x33333333);
	unsigned int numb = (((((numa >> 4) & 0xF0F0F0F) + (numa & 0xF0F0F0F)) >> 8) & 0xFF00FF)
		+ ((((numa >> 4) & 0xF0F0F0F) + (numa & 0xF0F0F0F)) & 0xFF00FF);

	return (numb >> 16) + (numb & 0xFFFF);
}