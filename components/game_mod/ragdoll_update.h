#pragma once

struct RagdollSortStruct
{
	float distSq;
	RagdollBody *body;
};

void Ragdoll_GetTorsoPosition(RagdollBody *body, float *center);
void Ragdoll_ExplosionEvent(int localClientNum, bool isCylinder, const float *origin, float innerRadius, float outerRadius, const float *impulse, float inScale);
bool Ragdoll_BodyNewState(RagdollBody *body, RagdollBodyState state);