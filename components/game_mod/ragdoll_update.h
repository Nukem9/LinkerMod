#pragma once

struct RagdollSortStruct
{
	float distSq;
	RagdollBody *body;
};

void Ragdoll_ExplosionEvent(int localClientNum, bool isCylinder, const float *origin, float innerRadius, float outerRadius, const float *impulse, float inScale);