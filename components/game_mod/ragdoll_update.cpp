#include "stdafx.h"

bool R_CullPoint(int localClient, const float *p0, float cutoff)
{
	return ((bool(__cdecl *)(int, const float *, float))0x006B63D0)(localClient, p0, cutoff);
}

// /ragdoll/ragdoll_update.cpp:913
void Ragdoll_GetTorsoPosition(RagdollBody *body, float *center)
{
	ASSERT(body && Ragdoll_BodyInUse(body));

	BoneOrientation *orientations = Ragdoll_BodyBoneOrientations(body);
	center[0] = orientations[0].origin[0];
	center[1] = orientations[0].origin[1];
	center[2] = orientations[0].origin[2];
}

// /ragdoll/ragdoll_update.cpp:940
void Ragdoll_ExplosionEvent(int localClientNum, bool isCylinder, const float *origin, float innerRadius, float outerRadius, const float *impulse, float inScale)
{
	ASSERT(origin);
	ASSERT(innerRadius >= 0.0f);
	ASSERT(outerRadius >= innerRadius);

	if (localClientNum != 0)
		return;

	if (outerRadius == 0.0f)
		return;

	float cutoffDistSqr = ragdoll_reactivation_cutoff->current.value * ragdoll_reactivation_cutoff->current.value;
	float outerRadiusSqr = outerRadius * outerRadius;
	float innerRadiusSqr = innerRadius * innerRadius;
	float invRange = 0.0f;

	if (outerRadiusSqr > innerRadiusSqr)
		invRange = 1.0f / (outerRadiusSqr - innerRadiusSqr);

	RagdollSortStruct hitEntsSorter[64];
	int hitCount = 0;

	for (auto i = g_ragdoll_body_pool->begin(); i != g_ragdoll_body_pool->end(); i++)
	{
		RagdollBody *body = (*i);

		ASSERT(Ragdoll_BodyInUse(body));

		float ragdollOrigin[3];
		Ragdoll_BodyRootOrigin(body, ragdollOrigin);

		hitEntsSorter[hitCount].distSq = Vec3DistanceSq(origin, ragdollOrigin);
		hitEntsSorter[hitCount++].body = body;
	}

	// Sort the ragdolled bodies by nearest distance
	std::sort(hitEntsSorter, hitEntsSorter + hitCount, [](RagdollSortStruct& ent1, RagdollSortStruct& ent2)
	{
		return ent2.distSq > ent1.distSq;
	});

	for (int i = 0; i < hitCount; i++)
	{
		RagdollBody *body = hitEntsSorter[i].body;

		ASSERT(Ragdoll_BodyInUse(body));

		if (body->state < BS_RUNNING)
			continue;

		float torsoPos[3];
		Ragdoll_GetTorsoPosition(body, torsoPos);

		auto cgameGlob	= CG_GetLocalClientGlobals(localClientNum);
		float dist		= Vec3DistanceSq((const float *)(cgameGlob + 0x8A388), torsoPos);

		if (dist <= cutoffDistSqr && !R_CullPoint(localClientNum, torsoPos, -100.0f))
		{
			float delta[3];
			if (isCylinder)
			{
				delta[0] = origin[0] - torsoPos[0];
				delta[1] = origin[1] - torsoPos[1];
				delta[2] = 0.0f;
			}
			else
			{
				delta[0] = torsoPos[0] - origin[0];
				delta[1] = torsoPos[1] - origin[1];
				delta[2] = torsoPos[2] - origin[2];
			}

			if ((Vec3Dot(delta, delta) < outerRadiusSqr) && (!Ragdoll_BodyIdle(body) || Ragdoll_BodyNewState(body, BS_RUNNING)))
			{
				body->stateMsec = 0;
				float boneScale = 1.0f / body->numBones;

				for (int boneIdx = 0; boneIdx < body->numBones; ++boneIdx)
				{
					float scale = inScale;
					Bone *bone = &body->bones[boneIdx];

					if (!bone->rigidBody)
						continue;

					float centerOfMass[3];
					Phys_ObjGetCenterOfMass(bone->rigidBody, centerOfMass);

					centerOfMass[0] += flrand(-1.0f, 1.0f);
					centerOfMass[1] += flrand(-1.0f, 1.0f);
					centerOfMass[2] += flrand(-1.0f, 1.0f);

					delta[0] = centerOfMass[0] - origin[0];
					delta[1] = centerOfMass[1] - origin[1];
					delta[2] = centerOfMass[2] - origin[2];

					float radiusSqr = Vec3Dot(delta, delta);

					if (radiusSqr < outerRadiusSqr)
					{
						if (radiusSqr > innerRadiusSqr)
						{
							ASSERT(outerRadiusSqr > innerRadiusSqr);
							scale *= ((outerRadiusSqr - radiusSqr) * invRange);
						}

						//
						// __asm movss xmm, vec3_origin
						// __asm lahf
						// if (__SETP__(_AH & 0x44, 0))
						//
						float hitForce[3];

						if (impulse[0] != 0.0f ||
							impulse[1] != 0.0f ||
							impulse[2] != 0.0f)
						{
							hitForce[0] = impulse[0];
							hitForce[1] = impulse[1];
							hitForce[2] = impulse[2];
						}
						else
						{
							hitForce[0] = delta[0];
							hitForce[1] = delta[1];
							hitForce[2] = delta[2];

							if (isCylinder)
								hitForce[2] = 0.0f;

							Vec3Normalize(hitForce);
						}

						hitForce[2] += ragdoll_explode_upbias->current.value;
						Vec3Normalize(hitForce);

						float explodeForce = (scale * ragdoll_explode_force->current.value) * boneScale;
						hitForce[0] *= explodeForce;
						hitForce[1] *= explodeForce;
						hitForce[2] *= explodeForce;

						Phys_ObjAddForce(bone->rigidBody, centerOfMass, hitForce, false);
					}
				}
			}
		}
	}
}

// /ragdoll/ragdoll_update.cpp:2128
bool Ragdoll_BodyNewState(RagdollBody *body, RagdollBodyState state)
{
	return ((bool(__cdecl *)(RagdollBody *, RagdollBodyState))0x0064A2D0)(body, state);
}