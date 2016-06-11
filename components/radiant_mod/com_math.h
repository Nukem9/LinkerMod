#pragma once

void Vec3Cross(const float *v0, const float *v1, float *cross);
void *Vec3Scale(const float *vec, float scale, float *dest);
float Vec3Normalize(float *v);

void PerpendicularVector(const float *src, float *dst);

void MatrixMultiply44(float *a, float *b, float *out);
void MatrixTranspose44(const float *in, float *out);
