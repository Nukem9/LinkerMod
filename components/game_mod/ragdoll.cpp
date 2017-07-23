#include "stdafx.h"

phys_free_list<RagdollBody> *g_ragdoll_body_pool = (phys_free_list<RagdollBody> *)0x00B79D38;

// /ragdoll/ragdoll.cpp:213
void Ragdoll_BodyRootOrigin(RagdollBody *body, float *origin)
{
	if (!Ragdoll_BodyPoseValid(body))
		return;

	BoneOrientation *orientations = Ragdoll_BodyBoneOrientations(body);
	origin[0] = orientations[0].origin[0];
	origin[1] = orientations[0].origin[1];
	origin[2] = orientations[0].origin[2];
}

BoneOrientation *Ragdoll_BodyBoneOrientations(RagdollBody *body)
{
	return ((BoneOrientation *(__cdecl *)(RagdollBody *))0x0054E920)(body);
}