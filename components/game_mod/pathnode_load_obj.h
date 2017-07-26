#pragma once

struct nodespawn_t
{
	char *name;
	nodeType type;
};

static nodespawn_t *nodespawns = (nodespawn_t *)0x00A55B78;

void Path_CreateNodes();
void Path_InitStatic(int restart);
void G_ParsePathnodeFields(SpawnVar *spawnVar, pathnode_t *node, nodeType type);
void SP_spawn_node(SpawnVar *spawnVar, nodeType type);
nodeType G_GetNodeTypeFromClassname(const char *classname);
void G_SpawnPathnodeStatic(SpawnVar *spawnVar, const char *classname);
void Path_BuildChains();
bool Path_FindOverlappingNodes();
void Path_LoadPaths();
bool Path_AttemptLink(pathnode_t *pNodeFrom, pathnode_t *pNodeTo, pathlink_s *pLinks, int iMaxLinks);
pathlink_s *G_GetNextAvailableTempLinks();
void Path_ConnectPathsForSingleNode(pathnode_t *node);
void Path_ConnectPaths();
void Path_SavePaths();