#include "stdafx.h"

int g_tempPathNodeLinksCount;
pathlink_s g_tempPathNodeLinks[MAX_LINKS_PER_NODE * MAX_PATH_DYNAMIC_NODES];

// /game/pathnode_load_obj.cpp:207
void Path_CreateNodes()
{
	pathstatic->pathLinks = nullptr;
	pathstatic->indirectNodes = nullptr;
	pathstatic->pathbuf = nullptr;

	gameWorldCurrent->path.nodes = (pathnode_t *)Hunk_Alloc(PATH_MAX_NODES * sizeof(pathnode_t), "Path_CreateNodes", 6);
	gameWorldCurrent->path.basenodes = (pathbasenode_t *)Hunk_Alloc(PATH_MAX_NODES * sizeof(pathbasenode_t), "Path_CreateNodes", 6);
}

// /game/pathnode_load_obj.cpp:221
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

// /game/pathnode_load_obj.cpp:324
void G_ParsePathnodeFields(SpawnVar *spawnVar, pathnode_t *node, nodeType type)
{
	((void(__cdecl *)(SpawnVar *, pathnode_t *, nodeType))0x00626C00)(spawnVar, node, type);
}

// /game/pathnode_load_obj.cpp:381
void SP_spawn_node(SpawnVar *spawnVar, nodeType type)
{
	static unsigned int overCount;

	ASSERT(type < NODE_NUMTYPES);
	ASSERT(gameWorldCurrent->path.nodes);

	// Check if we exceeded the hardcoded limit
	if (gameWorldCurrent->path.nodeCount >= (PATH_MAX_NODES - MAX_PATH_DYNAMIC_NODES))
	{
		Com_Printf(18, "PATH_MAX_NODES (%i) exceeded. Nodecount: %d\n", PATH_MAX_NODES, ++overCount + gameWorldCurrent->path.nodeCount);
		return;
	}

	// Otherwise parse the new index
	pathnode_t *loadNode = &gameWorldCurrent->path.nodes[gameWorldCurrent->path.nodeCount];
	memset(loadNode, 0, sizeof(pathnode_t));

	ASSERT(gameWorldCurrent->path.nodeCount == (unsigned short)gameWorldCurrent->path.nodeCount);

	G_ParsePathnodeFields(spawnVar, loadNode, type);
	gameWorldCurrent->path.nodeCount++;
}

// /game/pathnode_load_obj.cpp:407
nodeType G_GetNodeTypeFromClassname(const char *classname)
{
	for (nodespawn_t *ns = nodespawns; ns->name; ns++)
	{
		if (!strcmp(ns->name, classname))
			return ns->type;
	}

	Com_Error(ERR_DROP, "%s doesn't have a spawn function", classname);
	return NODE_PATHNODE;
}

// /game/pathnode_load_obj.cpp:430
void G_SpawnPathnodeStatic(SpawnVar *spawnVar, const char *classname)
{
	SP_spawn_node(spawnVar, G_GetNodeTypeFromClassname(classname));
}

// /game/pathnode_load_obj.cpp:557
void Path_BuildChains()
{
	((void(__cdecl *)())0x0047CA10)();
}

// /game/pathnode_load_obj.cpp:695
bool Path_FindOverlappingNodes()
{
	return ((bool(__cdecl *)())0x0048E0E0)();
}

// /game/pathnode_load_obj.cpp:1120
void Path_LoadPaths()
{
	((void(__cdecl *)())0x00653D40)();
}

// /game/pathnode_load_obj.cpp:1346
bool Path_AttemptLink(pathnode_t *pNodeFrom, pathnode_t *pNodeTo, pathlink_s *pLinks, int iMaxLinks)
{
	static DWORD dwCall = 0x00817AC0;

	__asm
	{
		push iMaxLinks
		push pLinks
		push pNodeTo
		mov eax, pNodeFrom
		call [dwCall]
		add esp, 0xC
	}
}

// /game/pathnode_load_obj.cpp:1603
pathlink_s *G_GetNextAvailableTempLinks()
{
	ASSERT(g_tempPathNodeLinksCount < MAX_PATH_DYNAMIC_NODES);

	pathlink_s *links = &g_tempPathNodeLinks[MAX_LINKS_PER_NODE * g_tempPathNodeLinksCount];
	g_tempPathNodeLinksCount++;

	memset(links, 0, MAX_LINKS_PER_NODE * sizeof(pathlink_s));
	return links;
}

// /game/pathnode_load_obj.cpp:1618
void Path_ConnectPathsForSingleNode(pathnode_t *node)
{
	if (g_tempPathNodeLinksCount >= MAX_PATH_DYNAMIC_NODES)
		return;

	if (node->constant.type == NODE_BADNODE)
		return;

	if (node->constant.spawnflags & 1)
		return;

	if (!node->constant.Links || !(node->constant.spawnflags & 0x4000))
	{
		node->constant.spawnflags |= 0x4000;
		node->constant.Links = G_GetNextAvailableTempLinks();
	}

	node->constant.totalLinkCount = 0;
	unsigned int nodeindex = Path_ConvertNodeToIndex(node);

	for (unsigned int i = 0; i < gameWorldCurrent->path.nodeCount; i++)
	{
		pathnode_t *othernode = &gameWorldCurrent->path.nodes[i];

		if (!(othernode->constant.spawnflags & 1) && othernode->constant.type != NODE_BADNODE && othernode != node)
		{
			for (unsigned short j = 0; j < othernode->constant.totalLinkCount; j++)
			{
				if (othernode->constant.Links[j].nodeNum == nodeindex)
				{
					othernode->constant.Links[j] = othernode->constant.Links[--othernode->constant.totalLinkCount];
					othernode->dynamic.wLinkCount = othernode->constant.totalLinkCount;
				}
			}

			if (Path_AttemptLink(node, othernode, node->constant.Links, MAX_LINKS_PER_NODE))
			{
				if (!othernode->constant.Links || !(othernode->constant.spawnflags & 0x4000))
				{
					othernode->constant.spawnflags |= 0x4000;

					pathlink_s *linksBuffer = G_GetNextAvailableTempLinks();

					if (othernode->constant.Links)
						memcpy(linksBuffer, othernode->constant.Links, othernode->constant.totalLinkCount * sizeof(pathlink_s));

					othernode->constant.Links = linksBuffer;
				}

				if (othernode->constant.totalLinkCount < MAX_LINKS_PER_NODE && Path_AttemptLink(othernode, node, othernode->constant.Links, MAX_LINKS_PER_NODE))
					othernode->dynamic.wLinkCount = othernode->constant.totalLinkCount;
			}

			if (node->constant.totalLinkCount >= MAX_LINKS_PER_NODE)
				break;
		}
	}

	node->dynamic.wLinkCount = node->constant.totalLinkCount;
}

// /game/pathnode_load_obj.cpp:1704
void Path_ConnectPaths()
{
	((void(__cdecl *)())0x004C4780)();
}

// /game/pathnode_load_obj.cpp:1856
void Path_SavePaths()
{
	((void(__cdecl *)())0x00462950)();
}