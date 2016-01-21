#include "Convert.h"
#include "Lightmaps.h"
#include "LightGridColors.h"
#include "BrushModels.h"
#include "PrimaryLights.h"
#include "ReflectionProbes.h"
#include "Lump.h"
#include "ConsoleLog.h"

int ConvertLump_WAWtoBO(Lump* wawLump, Lump* boLump, LUMP_TYPE type)
{
	switch(type)
	{
	case LUMP_MODELS:
		return ConvertLump_WAWtoBO_BrushModels(wawLump,boLump);
	case LUMP_PRIMARY_LIGHTS:
		return ConvertLump_WAWtoBO_PrimaryLights(wawLump,boLump);
	case LUMP_LIGHTBYTES:
		return ConvertLump_WAWtoBO_Lightmaps(wawLump,boLump);
	case LUMP_REFLECTION_PROBES:
		return ConvertLump_WAWtoBO_ReflectionProbes(wawLump,boLump);
	case LUMP_LIGHTGRIDCOLORS:
		return ConvertLump_WAWtoBO_LightGridColors(wawLump,boLump);

	case LUMP_MATERIALS:
	case LUMP_TRIANGLES:
	case LUMP_LIGHTGRIDENTRIES:
	case LUMP_PLANES:
	case LUMP_BRUSHSIDES:
	case LUMP_BRUSHSIDEEDGECOUNTS:
	case LUMP_BRUSHEDGES:
	case LUMP_BRUSHES:
	case LUMP_DRAWVERTS:
	case LUMP_DRAWINDICES:
	case LUMP_CULLGROUPS:
	case LUMP_CULLGROUPINDICES:
	case LUMP_PORTALVERTS:
	case LUMP_UINDS:
	case LUMP_BRUSHVERTSCOUNTS:
	case LUMP_BRUSHVERTS:
	case LUMP_AABBTREES:
	case LUMP_CELLS:
	case LUMP_PORTALS:
	case LUMP_NODES:
	case LUMP_LEAFS:
	case LUMP_LEAFBRUSHES:
	case LUMP_LEAFSURFACES:
	case LUMP_COLLISIONVERTS:
	case LUMP_COLLISIONTRIS:
	case LUMP_COLLISIONEDGEWALKABLE:
	case LUMP_COLLISIONBORDERS:
	case LUMP_COLLISIONPARTITIONS:
	case LUMP_COLLISIONAABBS:
	case LUMP_VISIBILITY:
	case LUMP_ENTITIES:
	case LUMP_PATHCONNECTIONS:
	case LUMP_VERTEX_LAYER_DATA:
	case LUMP_LIGHTGRIDHEADER:
	case LUMP_LIGHTGRIDROWS:
	case LUMP_WATERHEADER:
	case LUMP_WATERCELLS:
	case LUMP_WATERCELLDATA:
	case LUMP_BURNABLEHEADER:
	case LUMP_BURNABLECELLS:
	case LUMP_BURNABLECELLDATA:
	case LUMP_SIMPLELIGHTMAPBYTES:
	case LUMP_LODCHAINS:
	case LUMP_LODINFOS:
	case LUMP_LODSURFACES:
	case LUMP_LIGHTREGIONS:
	case LUMP_LIGHTREGION_HULLS:
	case LUMP_LIGHTREGION_AXES:
	case LUMP_WIILIGHTGRID:
	case LUMP_LIGHTGRID2D_LIGHTS:
	case LUMP_LIGHTGRID2D_INDICES:
	case LUMP_LIGHTGRID2D_POINTS:
	case LUMP_LIGHTGRID2D_CELLS:
	case LUMP_LIGHT_CORONAS:
	{
		*boLump = *wawLump;
		return 0;
	}

	default:
		boLump->FreeMemory();
		return 1;
	}
}

int ConvertBSP_WAWtoBO(D3DBSP* wawBSP, D3DBSP* boBSP)
{
	boBSP->diskLumpOrderSize = wawBSP->diskLumpOrderSize;
	boBSP->diskLumpOrder = new LUMP_TYPE[boBSP->diskLumpOrderSize];
	memcpy(boBSP->diskLumpOrder,wawBSP->diskLumpOrder,sizeof(LUMP_TYPE)*wawBSP->diskLumpOrderSize);

	boBSP->diskLumpCount = 0;
	for(DWORD typeIndex = 0; typeIndex < INFO_LUMP_TYPE_COUNT; typeIndex++)
	{
		if(!wawBSP->lumps[typeIndex].isEmpty)
		{
			ConvertLump_WAWtoBO(&wawBSP->lumps[typeIndex],&boBSP->lumps[typeIndex], (LUMP_TYPE)typeIndex);
			boBSP->diskLumpCount += !boBSP->lumps[typeIndex].isEmpty;
		}
	}

	return 0;
}

int ConvertLump_BOtoWAW(Lump* boLump, Lump* wawLump, LUMP_TYPE type)
{
	switch(type)
	{
	case LUMP_MODELS:
		return ConvertLump_BOtoWAW_BrushModels(boLump,wawLump);
	case LUMP_PRIMARY_LIGHTS:
		return ConvertLump_BOtoWAW_PrimaryLights(boLump,wawLump);
	case LUMP_LIGHTBYTES:
		return ConvertLump_BOtoWAW_Lightmaps(boLump,wawLump);
	case LUMP_REFLECTION_PROBES:
		return ConvertLump_BOtoWAW_ReflectionProbes(boLump,wawLump);
	case LUMP_LIGHTGRIDCOLORS:
		return ConvertLump_BOtoWAW_LightGridColors(boLump,wawLump);

	case LUMP_MATERIALS:
	case LUMP_TRIANGLES:
	case LUMP_LIGHTGRIDENTRIES:
	case LUMP_PLANES:
	case LUMP_BRUSHSIDES:
	case LUMP_BRUSHSIDEEDGECOUNTS:
	case LUMP_BRUSHEDGES:
	case LUMP_BRUSHES:
	case LUMP_DRAWVERTS:
	case LUMP_DRAWINDICES:
	case LUMP_CULLGROUPS:
	case LUMP_CULLGROUPINDICES:
	case LUMP_PORTALVERTS:
	case LUMP_UINDS:
	case LUMP_BRUSHVERTSCOUNTS:
	case LUMP_BRUSHVERTS:
	case LUMP_AABBTREES:
	case LUMP_CELLS:
	case LUMP_PORTALS:
	case LUMP_NODES:
	case LUMP_LEAFS:
	case LUMP_LEAFBRUSHES:
	case LUMP_LEAFSURFACES:
	case LUMP_COLLISIONVERTS:
	case LUMP_COLLISIONTRIS:
	case LUMP_COLLISIONEDGEWALKABLE:
	case LUMP_COLLISIONBORDERS:
	case LUMP_COLLISIONPARTITIONS:
	case LUMP_COLLISIONAABBS:
	case LUMP_VISIBILITY:
	case LUMP_ENTITIES:
	case LUMP_PATHCONNECTIONS:
	case LUMP_VERTEX_LAYER_DATA:
	case LUMP_LIGHTGRIDHEADER:
	case LUMP_LIGHTGRIDROWS:
	case LUMP_WATERHEADER:
	case LUMP_WATERCELLS:
	case LUMP_WATERCELLDATA:
	case LUMP_BURNABLEHEADER:
	case LUMP_BURNABLECELLS:
	case LUMP_BURNABLECELLDATA:
	case LUMP_SIMPLELIGHTMAPBYTES:
	case LUMP_LODCHAINS:
	case LUMP_LODINFOS:
	case LUMP_LODSURFACES:
	case LUMP_LIGHTREGIONS:
	case LUMP_LIGHTREGION_HULLS:
	case LUMP_LIGHTREGION_AXES:
	case LUMP_WIILIGHTGRID:
	case LUMP_LIGHTGRID2D_LIGHTS:
	case LUMP_LIGHTGRID2D_INDICES:
	case LUMP_LIGHTGRID2D_POINTS:
	case LUMP_LIGHTGRID2D_CELLS:
	case LUMP_LIGHT_CORONAS:
	{
		*wawLump = *boLump;
		return 0;
	}

	default:
		wawLump->FreeMemory();
		return 1;
	}
}

int ConvertBSP_BOtoWAW(D3DBSP* boBSP, D3DBSP* wawBSP)
{
	wawBSP->diskLumpOrderSize = boBSP->diskLumpOrderSize;
	wawBSP->diskLumpOrder = new LUMP_TYPE[wawBSP->diskLumpOrderSize];
	memcpy(wawBSP->diskLumpOrder,boBSP->diskLumpOrder,sizeof(LUMP_TYPE)*boBSP->diskLumpOrderSize);

	wawBSP->diskLumpCount = 0;
	for(DWORD typeIndex = 0; typeIndex < INFO_LUMP_TYPE_COUNT; typeIndex++)
	{
		if(!boBSP->lumps[typeIndex].isEmpty)
		{
			ConvertLump_BOtoWAW(&boBSP->lumps[typeIndex],&wawBSP->lumps[typeIndex], (LUMP_TYPE)typeIndex);
			wawBSP->diskLumpCount += !wawBSP->lumps[typeIndex].isEmpty;
		}
	}

	return 0;
}

int ConvertBSP_Copy(D3DBSP* inputBSP, D3DBSP* targetBSP)
{
	targetBSP->bspVersion = inputBSP->bspVersion;

	targetBSP->diskLumpCount = inputBSP->diskLumpCount;
	targetBSP->diskLumpOrderSize = inputBSP->diskLumpOrderSize;
	targetBSP->diskLumpOrder = new LUMP_TYPE[inputBSP->diskLumpOrderSize];
	memcpy(targetBSP->diskLumpOrder,inputBSP->diskLumpOrder,sizeof(LUMP_TYPE)*inputBSP->diskLumpOrderSize);

	for(DWORD typeIndex = 0; typeIndex < INFO_LUMP_TYPE_COUNT; typeIndex++)
	{
		if(!inputBSP->lumps[typeIndex].isEmpty)
		{
			targetBSP->lumps[typeIndex] = inputBSP->lumps[typeIndex];
		}
		else
		{
			targetBSP->lumps[typeIndex].FreeMemory();
		}
	}

	return 0;
}