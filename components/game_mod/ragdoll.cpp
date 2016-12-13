#include "stdafx.h"

phys_free_list<RagdollBody> *g_ragdoll_body_pool = (phys_free_list<RagdollBody> *)0x00B79D38;

void Ragdoll_BodyRootOrigin(RagdollBody *body, float *origin)
{
	((void(__cdecl *)(RagdollBody *, float *))0x00554310)(body, origin);
}

bool Ragdoll_BodyInUse(RagdollBody *body)
{
	ASSERT(body);
	return body->references > 0;
}

bool Ragdoll_BodyIdle(RagdollBody *body)
{
	ASSERT(body);
	return body->state == BS_IDLE;
}

BoneOrientation *Ragdoll_BodyBoneOrientations(RagdollBody *body)
{
	return ((BoneOrientation *(__cdecl *)(RagdollBody *))0x0054E920)(body);
}