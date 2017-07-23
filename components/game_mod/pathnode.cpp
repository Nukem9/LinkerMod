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
		ASSERT_MSG_VA(!link->disconnectCount, "%d, %d, %d", Path_ConvertNodeToIndex(node), j, link->nodeNum);
	}
}

// /game/pathnode.cpp:5054
void Path_ValidateAllNodes()
{
	// for (int i = 0; i < g_path_actualNodeCount; ++i)
	//	Path_ValidateNode(&gameWorldCurrent->path.nodes[i]);
}

// /game/pathnode.cpp:5996
pathnode_t *G_FindPathNode(SpawnVar *spawnVar, nodeType type, const int gameId)
{
	pathnode_t compareNode;
	memset(&compareNode, 0, sizeof(pathnode_t));

	G_ParsePathnodeFields(spawnVar, &compareNode, type);

	pathnode_t *bestNode = nullptr;
	int bestScore = 1;

	for (unsigned int i = 0; i < gameWorldCurrent->path.nodeCount; ++i)
	{
		int score = 0;
		pathnode_t *node = &gameWorldCurrent->path.nodes[i];

		// 2D distance, ignores height
		float dist =
			((compareNode.constant.vOrigin[0] - node->constant.vOrigin[0]) *
			(compareNode.constant.vOrigin[0] - node->constant.vOrigin[0])) +
				((compareNode.constant.vOrigin[1] - node->constant.vOrigin[1]) *
			(compareNode.constant.vOrigin[1] - node->constant.vOrigin[1]));

		// The closer the distance, the higher the base match score
		if (dist < 100.0f)
			score = (dist < 0.5f) ? 2 : 1;

		if (node->constant.target && node->constant.target == compareNode.constant.target)
			++score;

		if (node->constant.targetname && node->constant.targetname == compareNode.constant.targetname)
			++score;

		if (node->constant.script_noteworthy && node->constant.script_noteworthy == compareNode.constant.script_noteworthy)
			++score;

		if (score > bestScore)
		{
			bestNode = &gameWorldCurrent->path.nodes[i];
			bestScore = score;
		}
	}

	// If the search failed, check if there was a previous mapping
	if (gameId > 0)
	{
		if ((unsigned int)gameId < gameWorldCurrent->path.nodeCount && !bestNode)
			bestNode = &gameWorldCurrent->path.nodes[gameId];
	}

	return bestNode;
}

// /game/pathnode.cpp:6061
void G_ProcessPathnodeCommand(RadiantCommand *command, SpawnVar *spawnVar)
{
	const char *classname = GetPairValue(spawnVar, "classname");
	nodeType nodetype = G_GetNodeTypeFromClassname(classname);

	RadiantCommandType commandType = command->type;
	int gameId = G_GetGameIdMapping(command->liveUpdateId);

	// Create a new node
	if (commandType == RADIANT_COMMAND_CREATE)
	{
		G_SpawnPathnodeStatic(spawnVar, classname);
		G_SpawnPathnodeDynamic(spawnVar);

		g_radiant_selected_pathnode = &gameWorldCurrent->path.nodes[gameWorldCurrent->path.nodeCount - 1];

		unsigned int nodeIndex = Path_ConvertNodeToIndex(g_radiant_selected_pathnode);
		G_DropPathNodeToFloor(nodeIndex);

		Path_ConnectPathsForSingleNode(g_radiant_selected_pathnode);

		Com_Printf(5, "Radiant Live Update: Created new path node\n");
		G_AssignGameIdMapping(command->liveUpdateId, nodeIndex);
	}
	else if (commandType == RADIANT_COMMAND_DELETE)
	{
		// Delete the node. Remove all node links and then set it to invalid status
		g_radiant_selected_pathnode = G_FindPathNode(spawnVar, nodetype, gameId);

		if (g_radiant_selected_pathnode)
		{
			int nodeindex = Path_ConvertNodeToIndex(g_radiant_selected_pathnode);

			for (unsigned int i = 0; i < gameWorldCurrent->path.nodeCount; i++)
			{
				pathnode_t *othernode = &gameWorldCurrent->path.nodes[i];

				for (unsigned short j = 0; j < othernode->constant.totalLinkCount; j++)
				{
					if (othernode->constant.Links[j].nodeNum == nodeindex)
					{
						othernode->constant.Links[j] = othernode->constant.Links[--othernode->constant.totalLinkCount];
						othernode->dynamic.wLinkCount = othernode->constant.totalLinkCount;
					}
				}
			}

			g_radiant_selected_pathnode->constant.totalLinkCount = 0;
			g_radiant_selected_pathnode->dynamic.wLinkCount = 0;
			g_radiant_selected_pathnode->constant.type = NODE_BADNODE;
		}

		g_radiant_selected_pathnode = nullptr;
	}
	else
	{
		// Update a currently existing node
		pathnode_t *node = nullptr;

		if (commandType == RADIANT_COMMAND_SELECT)
		{
			node = g_radiant_selected_pathnode = G_FindPathNode(spawnVar, nodetype, gameId);

			if (!node)
				Com_Printf(5, "Radiant Live Update: Can't find pathnode. Maps out of sync (Radiant/Game), re-bsp!\n");
		}
		else if (commandType == RADIANT_COMMAND_UPDATE_SELECTED)
		{
			node = g_radiant_selected_pathnode;
		}
		else if (commandType == RADIANT_COMMAND_UPDATE)
		{
			node = G_FindPathNode(spawnVar, nodetype, gameId);
		}

		if (node)
		{
			// Prevent the temporary link flag from getting lost when parsing SpawnVars
			unsigned short tempLinks = node->constant.spawnflags & 0x4000;

			G_ParsePathnodeFields(spawnVar, node, nodetype);

			if (tempLinks)
				node->constant.spawnflags |= 0x4000u;

			// Node attributes have been updated. Force it back to ground level.
			unsigned int nodeIndex = Path_ConvertNodeToIndex(node);
			G_DropPathNodeToFloor(nodeIndex);

			Path_ConnectPathsForSingleNode(node);
			G_AssignGameIdMapping(command->liveUpdateId, nodeIndex);
		}
	}
}

// /game/pathnode.cpp:6159
void G_ClearSelectedPathNode()
{
	g_radiant_selected_pathnode = nullptr;
}