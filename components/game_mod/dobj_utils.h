#pragma once

#define DOBJ_MAX_SUBMODELS 32
#define DOBJ_MAX_SURFS 128

struct XAnimTree_s;

struct DSkelPartBits
{
	int anim[5];
	int control[5];
	int skel[5];
};

struct DSkel
{
	DSkelPartBits partBits;
	int timeStamp;

	union
	{
		struct DObjAnimMat *mat;
		struct DObjAnimMat *localMat;
	};
};

struct DObj
{
	union
	{
		struct XAnimTree_s *localTree;
		struct XAnimTree_s *tree;
	};

	unsigned __int16 duplicateParts;
	unsigned __int16 entnum;
	char duplicatePartsSize;
	char numModels;
	char numBones;
	unsigned int ignoreCollision;
	volatile int locked;
	DSkel skel;
	float radius;
	unsigned int hidePartBits[5];
	char localClientIndex;
	unsigned char flags;
	unsigned char *ikState;

	union
	{
		XModel **localModels;
		XModel **models;
	};
};

int DObjGetNumModels(DObj *obj);
XModel *DObjGetModel(DObj *obj, int modelIndex);
void DObjGetHidePartBits(DObj *obj, unsigned int *partBits);
