#pragma once

const char *XModelGetName(XModel *model);
int XModelGetSurfaces(XModel *model, XSurface **surfaces, int submodel);
Material * const *XModelGetSkins(XModel *model, int lod);