#include "stdafx.h"

// /xanim/dobj_utils.cpp:66
int DObjGetNumModels(DObj *obj)
{
	return obj->numModels;
}

// /xanim/dobj_utils.cpp:504
XModel *DObjGetModel(DObj *obj, int modelIndex)
{
	ASSERT(obj);

	if (modelIndex < 0 || modelIndex >= obj->numModels)
		return nullptr;

	return obj->localModels[modelIndex];
}

// /xanim/dobj_utils.cpp:1139
void DObjGetHidePartBits(DObj *obj, unsigned int *partBits)
{
	partBits[0] = obj->hidePartBits[0];
	partBits[1] = obj->hidePartBits[1];
	partBits[2] = obj->hidePartBits[2];
	partBits[3] = obj->hidePartBits[3];
	partBits[4] = obj->hidePartBits[4];
}