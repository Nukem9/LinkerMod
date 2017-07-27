#include "stdafx.h"

// /xanim/xmodel_utils.cpp:78
const char *XModelGetName(XModel *model)
{
	ASSERT(model);

	return model->name;
}

#if 0
// /xanim/xmodel_utils.cpp:94
int XModelGetSurfaces(XModel *model, XSurface **surfaces, int submodel)
{
	ASSERT(model);
	ASSERT(surfaces);
	ASSERT(submodel >= 0);

	ASSERT_MSG(submodel < MAX_LODS, "submodel doesn't index MAX_LODS");
	ASSERT_MSG(model->lodInfo[submodel].surfIndex > 0 && model->lodInfo[submodel].surfIndex < model->numsurfs, "lodInfo->surfIndex doesn't index model->numsurfs");
	ASSERT(lodInfo->surfIndex + lodInfo->numsurfs <= model->numsurfs);

	*surfaces = &model->surfs[model->lodInfo[submodel].surfIndex];
	return model->lodInfo[submodel].numsurfs;
}

// /xanim/xmodel_utils.cpp:292
Material * const *XModelGetSkins(XModel *model, int lod)
{
	ASSERT(model);
	ASSERT(lod >= 0);

	return &model->materialHandles[model->lodInfo[lod].surfIndex];
}
#endif