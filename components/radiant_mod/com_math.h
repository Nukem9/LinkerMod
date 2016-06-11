#pragma once

struct vec4
{
	union
	{
		float data[4];
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
	};
};

void Vec3Cross(const float *v0, const float *v1, float *cross);
void Vec3Scale(const float *vec, float scale, float *dest);
void Vec3Subtract(const float *v0, const float* v1, float *dest);
float Vec3Normalize(float *v);

void PerpendicularVector(const float *src, float *dst);

void MatrixMultiply44(float *a, float *b, float *out);
void MatrixTranspose44(const float *in, float *out);
