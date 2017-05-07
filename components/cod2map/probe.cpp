#include "stdafx.h"
#include "../D3DBSP_Lib/D3DBSP_Lib/ReflectionProbes.h"

typedef DiskGfxReflectionProbe_BO Probe;

static bool g_shouldAddDebugSpheres = false;

int Probe_EnableDebugSpheres_f(int argc, const char **argv)
{
	g_shouldAddDebugSpheres = true;
	return 1;
}

bool Probe_UseDebugSpheres(void)
{
	return g_shouldAddDebugSpheres;
}

int Probe_AddDebugSpheres(D3DBSP* bsp)
{
	if (bsp == NULL)
		return -1;

	Lump& lump_probes = bsp->lumps[LUMP_REFLECTION_PROBES];
	Lump& lump_ents = bsp->lumps[LUMP_ENTITIES];

	if (lump_probes.isEmpty || lump_ents.isEmpty)
		return 0;

	std::string str = (char*)lump_ents.content;
	lump_ents.FreeMemory();

	char buf[1024];
	const char* fmt =
		"{\n"
		"\"classname\" \"misc_model\"\n"
		"\"model\" \"%s\"\n"
		"\"origin\" \"%f %f %f\"\n"
		"}\n";

	Probe* probe = (Probe*)lump_probes.content;
	for (unsigned int i = 0; i < lump_probes.size / sizeof(Probe); i++)
	{
		buf[0] = '\0';
		sprintf_s(buf, fmt, "test_sphere_silver", probe[i].origin[0], probe[i].origin[1], probe[i].origin[2]);
		str += buf;
	}

	lump_ents.AllocateMemory(str.size());
	memcpy(lump_ents.content, str.c_str(), str.size());

	return 0;
}
