#include "stdafx.h"

// /game/pathnode.cpp:1332
void node_droptofloor(pathnode_t *node)
{
	static DWORD dwCall = 0x00815190;

	__asm
	{
		mov eax, node
		call [dwCall]
	}
}

// /game/pathnode.cpp:1447
void G_InitPathBaseNode(pathbasenode_t *pbnode, pathnode_t *pnode)
{
	pbnode->vOrigin[0] = pnode->constant.vOrigin[0];
	pbnode->vOrigin[1] = pnode->constant.vOrigin[1];
	pbnode->vOrigin[2] = pnode->constant.vOrigin[2];
	pbnode->type = 1 << pnode->constant.type;

	if (pnode->constant.spawnflags & 1)
		pbnode->type |= 0x200000;
}

// /game/pathnode.cpp:1461
void G_DropPathNodeToFloor(unsigned int nodeIndex)
{
	node_droptofloor(&gameWorldCurrent->path.nodes[nodeIndex]);
	G_InitPathBaseNode(&gameWorldCurrent->path.basenodes[nodeIndex], &gameWorldCurrent->path.nodes[nodeIndex]);
}

// /game/pathnode.cpp:1505
void G_SpawnPathnodeDynamic(SpawnVar *spawnVar)
{
	((void(__cdecl *)(SpawnVar *))0x004B08E0)(spawnVar);
}

// /game/pathnode.cpp:1948
unsigned int Path_ConvertNodeToIndex(pathnode_t *node)
{
	unsigned int nodeIndex = node - gameWorldCurrent->path.nodes;

	ASSERT(node);
	// ASSERT(nodeIndex < g_path_actualNodeCount);

	return nodeIndex;
}

// /game/pathnode.cpp:1973
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

void Path_InitBadPlaces()
{
	memset(g_badplaces, 0, MAX_BADPLACES * sizeof(badplace_t));
}

// /game/pathnode.cpp:2289
void Path_InitLinkCounts()
{
	((void(__cdecl *)())0x00814190)();
}

// /game/pathnode.cpp:2327
void Path_InitLinkInfoArray()
{
	((void(__cdecl *)())0x00814260)();
}

// /game/pathnode.cpp:2387
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

// /game/pathnode.cpp:2471
void Path_CheckSpawnExitNodesConnectivity()
{
	// This function only applies to multiplayer (dog actors must have map exit nodes)
}

// /game/pathnode.cpp:5034
void Path_ValidateNode(pathnode_t *node)
{
	int j = j = node->constant.totalLinkCount - 1;

	for (; j >= node->dynamic.wLinkCount; --j)
	{
		pathlink_s *link = &node->constant.Links[j];
		ASSERT(link->disconnectCount > 0);
	}

	ASSERT(j == node->dynamic.wLinkCount - 1);

	for (; j >= 0; --j)
	{
		pathlink_s *link = &node->constant.Links[j];
		ASSERT_MSG(!link->disconnectCount, va("%d, %d, %d", Path_ConvertNodeToIndex(node), j, link->nodeNum));
	}
}

// /game/pathnode.cpp:5054
void Path_ValidateAllNodes()
{
	// for (int i = 0; i < g_path_actualNodeCount; ++i)
	//	Path_ValidateNode(&gameWorldCurrent->path.nodes[i]);
}