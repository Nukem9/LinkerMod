#pragma once

#define IS_NAN(x) ((*(unsigned int *)(&x) & 0x7F800000) == 0x7F800000)

float Vec3DistanceSq(const float *p1, const float *p2);
void Vec3Normalize(float *v);
float flrand(float min, float max);