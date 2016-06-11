#include "stdafx.h"

void Vec3Cross(const float *v0, const float *v1, float *cross)
{
	cross[0] = v0[1] * v1[2] - v0[2] * v1[1];
	cross[1] = v0[2] * v1[0] - v0[0] * v1[2];
	cross[2] = v0[0] * v1[1] - v0[1] * v1[0];
}

void *Vec3Scale(const float *vec, float scale, float *dest)
{
	dest[0] = scale * vec[0];
	dest[1] = scale * vec[1];
	dest[2] = scale * vec[2];
}

float Vec3Normalize(float *v)
{
	float length = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	float m = (-length < 0.0f) ? length : 1.0f;

	float invLen = 1.0f / m;
	v[0] = v[0] * invLen;
	v[1] = v[1] * invLen;
	v[2] = v[2] * invLen;
	return length;
}

void PerpendicularVector(const float *src, float *dst)
{
	float srcSq[3];
	srcSq[0] = src[0] * src[0];
	srcSq[1] = src[1] * src[1];
	srcSq[2] = src[2] * src[2];

	int pos = srcSq[0] > srcSq[1];
	if (srcSq[pos] > srcSq[2])
		pos = 2;

	float d = -src[pos];
	dst[0] = d * src[0];
	dst[1] = d * src[1];
	dst[2] = d * src[2];

	dst[pos] = dst[pos] + 1.0f;

	Vec3Normalize(dst);
}

void MatrixMultiply44(float *a, float *b, float *out)
{
	ASSERT(a != out);
	ASSERT(b != out);

	out[0] = a[0] * b[0] + a[1] * b[4] + a[2] * b[8] + a[3] * b[12];
	out[1] = a[0] * b[1] + a[1] * b[5] + a[2] * b[9] + a[3] * b[13];
	out[2] = a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + a[3] * b[14];
	out[3] = a[0] * b[3] + a[1] * b[7] + a[2] * b[11] + a[3] * b[15];
	out[4] = a[4] * b[0] + a[5] * b[4] + a[6] * b[8] + a[7] * b[12];
	out[5] = a[4] * b[1] + a[5] * b[5] + a[6] * b[9] + a[7] * b[13];
	out[6] = a[4] * b[2] + a[5] * b[6] + a[6] * b[10] + a[7] * b[14];
	out[7] = a[4] * b[3] + a[5] * b[7] + a[6] * b[11] + a[7] * b[15];
	out[8] = a[8] * b[0] + a[9] * b[4] + a[10] * b[8] + a[11] * b[12];
	out[9] = a[8] * b[1] + a[9] * b[5] + a[10] * b[9] + a[11] * b[13];
	out[10] = a[8] * b[2] + a[9] * b[6] + a[10] * b[10] + a[11] * b[14];
	out[11] = a[8] * b[3] + a[9] * b[7] + a[10] * b[11] + a[11] * b[15];
	out[12] = a[12] * b[0] + a[13] * b[4] + a[14] * b[8] + a[15] * b[12];
	out[13] = a[12] * b[1] + a[13] * b[5] + a[14] * b[9] + a[15] * b[13];
	out[14] = a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14];
	out[15] = a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15];
}

void MatrixTranspose44(const float *in, float *out)
{
	ASSERT(in != out);

	out[0] = in[0];
	out[1] = in[4];
	out[2] = in[8];
	out[3] = in[12];

	out[4] = in[1];
	out[5] = in[5];
	out[6] = in[9];
	out[7] = in[13];

	out[8] = in[2];
	out[9] = in[6];
	out[10] = in[10];
	out[11] = in[14];

	out[12] = in[3];
	out[13] = in[7];
	out[14] = in[11];
	out[15] = in[15];
}
