#include "stdafx.h"

phys_free_list<RagdollBody> *g_ragdoll_body_pool = (phys_free_list<RagdollBody> *)0x00B79D38;

bool Ragdoll_BodyPoseValid(RagdollBody *body)
{
	ASSERT(body);
	return body->state >= BS_TUNNEL_TEST;
}

void Ragdoll_BodyRootOrigin(RagdollBody *body, float *origin)
{
	if (!Ragdoll_BodyPoseValid(body))
		return;

	BoneOrientation *orientations = Ragdoll_BodyBoneOrientations(body);
	origin[0] = orientations[0].origin[0];
	origin[1] = orientations[0].origin[1];
	origin[2] = orientations[0].origin[2];
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