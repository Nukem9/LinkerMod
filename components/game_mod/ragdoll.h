#pragma once

enum RagdollBodyState
{
	BS_DEAD = 0x0,
	BS_DOBJ_WAIT = 0x1,
	BS_VELOCITY_CAPTURE = 0x2,
	BS_TUNNEL_TEST = 0x3,
	BS_RUNNING = 0x4,
	BS_IDLE = 0x5,
	RAGDOLL_NUM_STATES = 0x6,
};

struct Bone
{
	int parentBone;
	char animBones[2];
	int rigidBody;
	float length;
	float center[3];
};
static_assert(sizeof(Bone) == 0x1C, "Size check");
static_assert(offsetof(Bone, rigidBody) == 0x8, "Offset check");

struct RagdollBody
{
	int references;
	int ragdollDef;
	int dobj;
	/*DObj*/int *obj;
	/*cpose_t*/int *pose;
	float poseOffset[3];
	int localClientNum;
	RagdollBodyState state;
	int stateMsec;
	int stateFrames;
	int velCaptureMsec;
	int stableTime;
	int numBones;
	Bone bones[14];
	/* omitted */
};
static_assert(offsetof(RagdollBody, state) == 0x24, "Offset check");
static_assert(offsetof(RagdollBody, stateMsec) == 0x28, "Offset check");
static_assert(offsetof(RagdollBody, numBones) == 0x38, "Offset check");
static_assert(offsetof(RagdollBody, bones) == 0x3C, "Offset check");

struct BoneOrientation
{
	float origin[3];
	int boneFlags;
	float orientation[4];
};

template<typename T>
struct phys_free_list
{
	struct T_internal_base
	{
		T_internal_base *m_prev_T_internal;
		T_internal_base *m_next_T_internal;
	};

	struct T_internal : T_internal_base
	{
		T m_data;
		int m_ptr_list_index;
	};

	T_internal_base m_dummy_head;
	int m_list_count;
	int m_list_count_high_water;
	T *m_ptr_list[256];
	int m_ptr_list_count;
};

extern phys_free_list<RagdollBody> *g_ragdoll_body_pool;

void Ragdoll_BodyRootOrigin(RagdollBody *body, float *origin);
bool Ragdoll_BodyInUse(RagdollBody *body);
bool Ragdoll_BodyIdle(RagdollBody *body);
BoneOrientation *Ragdoll_BodyBoneOrientations(RagdollBody *body);