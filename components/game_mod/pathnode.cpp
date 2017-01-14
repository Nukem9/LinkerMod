#include "stdafx.h"

void Path_Init(int restart)
{
	if (true || !useFastFile->current.enabled)
	{
		memset(gameWorldCurrent, 0, sizeof(GameWorldMp));

		gameWorldCurrent->name = Com_GetHunkStringCopy(Dvar_GetString("mapname"));

		Path_InitStatic(restart);

		if (!restart)
			Path_CreateNodes();
	}

	memset(g_path, 0, sizeof(pathlocal_t));
	ASSERT(gameWorldCurrent->path.nodes);

	debugPath = nullptr;
	Path_InitBadPlaces();
}

void Path_CreateNodes()
{
	pathstatic->pathLinks		= nullptr;
	pathstatic->indirectNodes	= nullptr;
	pathstatic->pathbuf			= nullptr;

	gameWorldCurrent->path.nodes = (pathnode_t *)Hunk_Alloc(PATH_MAX_NODES * sizeof(pathnode_t), "Path_CreateNodes", 6);
	gameWorldCurrent->path.basenodes = (pathbasenode_t *)Hunk_Alloc(PATH_MAX_NODES * sizeof(pathbasenode_t), "Path_CreateNodes", 6);
}

void Path_InitStatic(int restart)
{
	if (restart)
	{
		// The game keeps existing node data when restarting a map
		gameWorldCurrent->path.nodeCount = 0;
	}
	else
	{
		// New map load. Initialize everything to zero.
		memset(&gameWorldCurrent->path, 0, sizeof(PathData));
	}

	g_pathsError = nullptr;
}

void Path_InitBadPlaces()
{
	memset(g_badplaces, 0, MAX_BADPLACES * sizeof(badplace_t));
}

void Path_BuildChains()
{
	((void(__cdecl *)())0x0047CA10)();
}

void Path_ConnectPaths()
{
	((void(__cdecl *)())0x004C4780)();
}

void Path_LoadPaths()
{
	((void(__cdecl *)())0x00653D40)();
}

void Path_InitLinkCounts()
{
	((void(__cdecl *)())0x00814190)();
}

void Path_InitLinkInfoArray()
{
	((void(__cdecl *)())0x00814260)();
}

bool Path_FindOverlappingNodes()
{
	return ((bool(__cdecl *)())0x0048E0E0)();
}

void Path_ValidateAllNodes()
{
	// TODO: Not implemented in the game itself
}

void Path_CheckSpawnExitNodesConnectivity()
{
	// TODO: Not implemented in the game itself
}

void Path_SavePaths()
{
	((void(__cdecl *)())0x00462950)();
}

void Path_InitPaths()
{
	if (true || !useFastFile->current.enabled)
	{
		Path_BuildChains();

		if (true || g_connectpaths->current.integer)
		{
			if (*(DWORD *)0x1C0706C == 2)
				Com_Error(ERR_DROP, "Attempting to load save game when g_connectpaths is set");

			Path_ConnectPaths();
		}
		else
			Path_LoadPaths();

		Path_InitLinkCounts();
	}

	Path_InitLinkInfoArray();

	if (!useFastFile->current.enabled && Path_FindOverlappingNodes() && g_connectpaths->current.integer >= 2)
		Com_Error(ERR_FATAL, "Overlapping path nodes. Check console log");

	Path_ValidateAllNodes();

	if (g_connectpaths->current.integer)
	{
		Path_CheckSpawnExitNodesConnectivity();
		Path_SavePaths();

		if (g_connectpaths->current.integer >= 2)
		{
			Sys_NormalExit();
			fflush(stdout);
			ExitProcess(0);
		}

		g_connectpaths->current.integer = 0;
		//Dvar_SetInt(g_connectpaths, 0);
	}
}