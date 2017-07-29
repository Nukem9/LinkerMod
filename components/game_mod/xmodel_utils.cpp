#include "stdafx.h"

// /xanim/xmodel_utils.cpp:78
const char *XModelGetName(XModel *model)
{
	ASSERT(model);

	return model->name;
}

int XModelNumBones(XModel *model)
{
	return model->numBones;
}

// /xanim/xmodel_utils.cpp:94
int XModelGetSurfaces(XModel *model, XSurface **surfaces, int submodel)
{
	ASSERT(model);
	ASSERT(surfaces);
	ASSERT_MSG(submodel >= 0 && submodel < MAX_LODS, "submodel doesn't index MAX_LODS");

	XModelLodInfo *lodInfo = &model->lodInfo[submodel];

	ASSERT_MSG(lodInfo->surfIndex > 0 && lodInfo->surfIndex < model->numsurfs, "lodInfo->surfIndex doesn't index model->numsurfs");
	ASSERT(lodInfo->surfIndex + lodInfo->numsurfs <= model->numsurfs);

	*surfaces = &model->surfs[model->lodInfo[submodel].surfIndex];
	return model->lodInfo[submodel].numsurfs;
}

// /xanim/xmodel_utils.cpp:292
Material * const *XModelGetSkins(XModel *model, int lod)
{
	ASSERT(model);
	ASSERT_MSG(lod >= 0 && lod < MAX_LODS, "lod doesn't index MAX_LODS");

	return &model->materialHandles[model->lodInfo[lod].surfIndex];
}