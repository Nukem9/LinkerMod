#include "stdafx.h"

void Phys_ObjAddForce(int id, const float *worldPos, const float *impulse, const bool relative)
{
	((void(__cdecl *)(int, const float *, const float *, const bool))0x00448900)(id, worldPos, impulse, relative);
}

void Phys_ObjGetCenterOfMass(int id, float *outPosition)
{
	((void(__cdecl *)(int, float *))0x0054E920)(id, outPosition);
}