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
STATIC_ASSERT_SIZE(Bone, 0x1C);
STATIC_ASSERT_OFFSET(Bone, rigidBody, 0x8);

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
STATIC_ASSERT_OFFSET(RagdollBody, state, 0x24);
STATIC_ASSERT_OFFSET(RagdollBody, stateMsec, 0x28);
STATIC_ASSERT_OFFSET(RagdollBody, numBones, 0x38);
STATIC_ASSERT_OFFSET(RagdollBody, bones, 0x3C);

struct BoneOrientation
{
	float origin[3];
	int boneFlags;
	float orientation[4];
};

template<typename T>
class phys_free_list
{
protected:
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

public:
	class iterator
	{
	private:
		T_internal_base *m_ptr;

	public:
		iterator(T_internal_base *Node) : m_ptr(Node)
		{
		}

		iterator operator++(int)
		{
			m_ptr = m_ptr->m_next_T_internal;
			return iterator(m_ptr);
		}

		bool operator!=(const iterator& rhs)
		{
			return this->m_ptr != rhs.m_ptr;
		}

		T *operator *()
		{
			return &((T_internal *)m_ptr)->m_data;
		}
	};

	iterator begin()
	{
		return m_dummy_head.m_next_T_internal;
	}

	iterator end()
	{
		return &m_dummy_head;
	}

private:
	T_internal_base m_dummy_head;
	int m_list_count;
	int m_list_count_high_water;
	T *m_ptr_list[256];
	int m_ptr_list_count;
};

extern phys_free_list<RagdollBody> *g_ragdoll_body_pool;

// /ragdoll/ragdoll.h:298
static bool Ragdoll_BodyPoseValid(RagdollBody *body)
{
	ASSERT(body);
	return body->state >= BS_TUNNEL_TEST;
}

// /ragdoll/ragdoll.h:312
static bool Ragdoll_BodyInUse(RagdollBody *body)
{
	ASSERT(body);
	return body->references > 0;
}

// /ragdoll/ragdoll.h:326
static bool Ragdoll_BodyIdle(RagdollBody *body)
{
	ASSERT(body);
	return body->state == BS_IDLE;
}

void Ragdoll_BodyRootOrigin(RagdollBody *body, float *origin);
BoneOrientation *Ragdoll_BodyBoneOrientations(RagdollBody *body);