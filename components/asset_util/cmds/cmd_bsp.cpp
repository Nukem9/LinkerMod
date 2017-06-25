#include "../sys/AppInfo.h"
#include "../common/io.h"
#include "../common/fs.h"

#include "../cvar.h"
#include "cmd_common.h"

#include "../D3DBSP_Lib/D3DBSP_Lib/D3DBSP_Lib.h"
#pragma comment(lib, "D3DBSP_Lib.lib")

static const char *LumpNames[] =
{
	"LUMP_MATERIALS",
	"LUMP_LIGHTBYTES",
	"LUMP_LIGHTGRIDENTRIES",
	"LUMP_LIGHTGRIDCOLORS",
	"LUMP_PLANES",
	"LUMP_BRUSHSIDES",
	"LUMP_BRUSHSIDEEDGECOUNTS",
	"LUMP_BRUSHEDGES",
	"LUMP_BRUSHES",
	"LUMP_TRIANGLES",
	"LUMP_DRAWVERTS",
	"LUMP_DRAWINDICES",
	"LUMP_CULLGROUPS",
	"LUMP_CULLGROUPINDICES",
	"LUMP_OBSOLETE_1",
	"LUMP_OBSOLETE_2",
	"LUMP_OBSOLETE_3",
	"LUMP_OBSOLETE_4",
	"LUMP_OBSOLETE_5",
	"LUMP_PORTALVERTS",
	"LUMP_OBSOLETE_6",
	"LUMP_UINDS",
	"LUMP_BRUSHVERTSCOUNTS",
	"LUMP_BRUSHVERTS",
	"LUMP_AABBTREES",
	"LUMP_CELLS",
	"LUMP_PORTALS",
	"LUMP_NODES",
	"LUMP_LEAFS",
	"LUMP_LEAFBRUSHES",
	"LUMP_LEAFSURFACES",
	"LUMP_COLLISIONVERTS",
	"LUMP_COLLISIONTRIS",
	"LUMP_COLLISIONEDGEWALKABLE",
	"LUMP_COLLISIONBORDERS",
	"LUMP_COLLISIONPARTITIONS",
	"LUMP_COLLISIONAABBS",
	"LUMP_MODELS",
	"LUMP_VISIBILITY",
	"LUMP_ENTITIES",
	"LUMP_PATHCONNECTIONS",
	"LUMP_REFLECTION_PROBES",
	"LUMP_VERTEX_LAYER_DATA",
	"LUMP_PRIMARY_LIGHTS",
	"LUMP_LIGHTGRIDHEADER",
	"LUMP_LIGHTGRIDROWS",
	"LUMP_OBSOLETE_10",
	"LUMP_UNLAYERED_TRIANGLES",
	"LUMP_UNLAYERED_DRAWVERTS",
	"LUMP_UNLAYERED_DRAWINDICES",
	"LUMP_UNLAYERED_CULLGROUPS",
	"LUMP_UNLAYERED_AABBTREES",
	"LUMP_WATERHEADER",
	"LUMP_WATERCELLS",
	"LUMP_WATERCELLDATA",
	"LUMP_BURNABLEHEADER",
	"LUMP_BURNABLECELLS",
	"LUMP_BURNABLECELLDATA",
	"LUMP_SIMPLELIGHTMAPBYTES",
	"LUMP_LODCHAINS",
	"LUMP_LODINFOS",
	"LUMP_LODSURFACES",
	"LUMP_LIGHTREGIONS",
	"LUMP_LIGHTREGION_HULLS",
	"LUMP_LIGHTREGION_AXES",
	"LUMP_WIILIGHTGRID",
	"LUMP_LIGHTGRID2D_LIGHTS",
	"LUMP_LIGHTGRID2D_INDICES",
	"LUMP_LIGHTGRID2D_POINTS",
	"LUMP_LIGHTGRID2D_CELLS",
	"LUMP_LIGHT_CORONAS",
	"LUMP_SHADOWMAP_VOLUMES",
	"LUMP_SHADOWMAP_VOLUME_PLANES",
	"LUMP_EXPOSURE_VOLUMES",
	"LUMP_EXPOSURE_VOLUME_PLANES",
	"LUMP_OCCLUDERS",
	"LUMP_OUTDOORBOUNDS",
	"LUMP_HERO_ONLY_LIGHTS"
};

extern unsigned int padded(unsigned int i);

int Cmd_BspInfo_f(int argc, char** argv)
{
	for (int i = 1; i < argc; i++)
	{
		char* filepath = argv[i];

		D3DBSP iBSP;
		if (iBSP.Load(argv[i]) != 0)
			continue;

		if (argc > 2)
			Con_Print("Info for: '%s'\n\n", FS_GetFilenameSubString(argv[i]));

		Con_Print("%-5s %-28s %-10s   %s\n\n", "LUMP", "NAME", "OFFSET", "SIZE");

		unsigned int lumpOffset = sizeof(DWORD) * 3 + 8 * iBSP.diskLumpCount;
		for (unsigned int j = 0; j < iBSP.diskLumpCount; j++)
		{
			LUMP_TYPE type = iBSP.diskLumpOrder[j];
			unsigned int size = iBSP.lumps[type].size;

			Con_Print("[%2d]: %-28s 0x%08X ( 0x%X bytes )\n", j, LumpNames[j], lumpOffset, size);

			lumpOffset += padded(size);
		}
	}

	if (argc < 2)
	{
		char* _argv[] = { NULL, "bsp_info" };
		Cmd_Help_f(ARRAYSIZE(_argv), _argv);
		return -1;
	}
	return 0;
}
