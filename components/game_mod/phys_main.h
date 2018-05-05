#pragma once

struct PhysPreset
{
	const char *name;
	int flags;
	float mass;
	float bounce;
	float friction;
	float bulletForceScale;
	float explosiveForceScale;
	const char *sndAliasPrefix;
	float piecesSpreadFraction;
	float piecesUpwardVelocity;
	int canFloat;
	float gravityScale;
	float centerOfMassOffset[3];
	float buoyancyBoxMin[3];
	float buoyancyBoxMax[3];
};

struct BodyState
{
	float position[3];
	float rotation[3][3];
	float velocity[3];
	float angVelocity[3];
	float centerOfMassOffset[3];
	float buoyancyBoxMin[3];
	float buoyancyBoxMax[3];
	float mass;
	float friction;
	float bounce;
	int timeLastAsleep;
	int id;
	int buoyancy;
	int underwater;
};

void Phys_ObjGetCenterOfMass(int id, float *outPosition);
void Phys_ObjAddForce(int id, const float *worldPos, const float *impulse, const bool relative);

void hk_Phys_ObjCreateAxis(); //int worldIndex, const float *position, struct gjk_geom_list_t* gjk_geom_list, int id, int do_collision_test);

typedef int (__cdecl* Phys_CreateBodyFromState_t)(int worldIndex, BodyState *state, struct gjk_geom_list_t *gjk_geom_list, const bool do_collision_test);
static Phys_CreateBodyFromState_t Phys_CreateBodyFromState = (Phys_CreateBodyFromState_t)0x005785C0;
