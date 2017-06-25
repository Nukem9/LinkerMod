#pragma once

struct XModel
{
	const char* name;
	// ...
};

typedef void *(__cdecl* Hunk_AllocXModelPrecache_t)(int size);
static Hunk_AllocXModelPrecache_t Hunk_AllocXModelPrecache = (Hunk_AllocXModelPrecache_t)0x00519FD0;
static Hunk_AllocXModelPrecache_t Hunk_AllocXModelPrecacheColl = (Hunk_AllocXModelPrecache_t)0x00519FE0;

bool XModelLoadConfigFile(const char **pos, int name, int config);

bool hk_XModelLoadConfigFile();

XModel *__cdecl XModelLoad(const char *name, void *(__cdecl *Alloc)(int), void *(__cdecl *AllocColl)(int));

XModel *__cdecl R_RegisterSkyboxModel(const char *name);
XModel *__cdecl XModelPrecache_Skybox(const char *name, void *(__cdecl *Alloc)(int), void *(__cdecl *AllocColl)(int));
XModel *__cdecl XModelPrecache_Skybox_LoadObj(const char *name, void *(__cdecl *Alloc)(int), void *(__cdecl *AllocColl)(int));
