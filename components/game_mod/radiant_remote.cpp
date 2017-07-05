#include "stdafx.h"

SOCKET g_ServerSocket = INVALID_SOCKET;
SOCKET g_ClientSocket = INVALID_SOCKET;

const char *GetPairValue(SpawnVar *spawnVar, const char *key)
{
	for (int i = 0; i < spawnVar->numSpawnVars; ++i)
	{
		if (!_stricmp(key, spawnVar->spawnVars[i][0]))
			return spawnVar->spawnVars[i][1];
	}

	return nullptr;
}

void RadiantRemoteInit()
{
	savedCommandCount = 0;
	memset(savedCommands, 0, sizeof(RadiantCommand) * 128);

	gCommandCount = 0;
	memset(gCommands, 0, sizeof(RadiantCommand) * 24);

	gObjectMappingCount = 0;
	memset(gObjectMapping, 0, sizeof(RadaintToGameMapping) * 128);

	// Initialize network and server socket (only if it hasn't been set up)
	if (!radiant_live->current.enabled || g_ServerSocket != INVALID_SOCKET)
		return;

	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
		Com_Error(ERR_FATAL, "LiveRadiant: Socket startup failed\n");

	// Create a TCP server socket
	g_ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (g_ServerSocket == INVALID_SOCKET)
		Com_Error(ERR_FATAL, "LiveRadiant: Socket creation failed\n");

	// Bind socket to any local address on port X
	sockaddr_in addrIn;
	addrIn.sin_family = AF_INET;
	addrIn.sin_port = htons(radiant_livePort->current.integer);
	addrIn.sin_addr.s_addr = inet_addr("127.0.0.1");

	Com_Printf(1, "LiveRadiant: Attempting to bind on port %d... ", (int)ntohs(addrIn.sin_port));

	if (bind(g_ServerSocket, (SOCKADDR *)&addrIn, sizeof(addrIn)) == SOCKET_ERROR)
	{
		Com_PrintError(1, "Failed to bind socket. Port in use?\n");

		RadiantRemoteShutdown();
		return;
	}

	// Listen for any number of incoming connections
	if (listen(g_ServerSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		Com_PrintError(1, "Failed to listen for incoming connections\n");

		RadiantRemoteShutdown();
		return;
	}

	Com_Printf(1, "Succeeded\n");
}

void RadiantRemoteShutdown()
{
	shutdown(g_ServerSocket, 2 /*SD_BOTH*/);
	closesocket(g_ServerSocket);

	shutdown(g_ClientSocket, 2 /*SD_BOTH*/);
	closesocket(g_ClientSocket);

	g_ServerSocket = INVALID_SOCKET;
	g_ClientSocket = INVALID_SOCKET;
}

void RadiantRemoteUpdate()
{
	if (!radiant_live->current.enabled || !RadiantRemoteUpdateSocket())
		return;

	// Non-blocking read
	RadiantCommand recvCommands[16];
	memset(recvCommands, 0, sizeof(recvCommands));

	int recvSize = recv(g_ClientSocket, (char *)&recvCommands, sizeof(recvCommands), 0);

	// Skip everything if there's no data
	if (recvSize == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
			return;

		// Some other problem occurred and now the socket is bad
		shutdown(g_ClientSocket, 2 /*SD_BOTH*/);
		closesocket(g_ClientSocket);

		g_ClientSocket = INVALID_SOCKET;
		return;
	}

	// Determine the number of commands sent, then tell the game
	size_t commandCount = recvSize / sizeof(RadiantCommand);

	Sys_EnterCriticalSection((CriticalSection)61);
	{
		for (size_t i = 0; i < commandCount; i++)
		{
			//
			// This must be enforced on the server side:
			// Classname must be supplied or the game will crash
			//
			if (recvCommands[i].type == RADIANT_COMMAND_SELECT)
			{
				if (!strstr(recvCommands[i].strCommand, "\"classname\""))
					continue;
			}

			// Print each network message to the console if enabled
			if (radiant_liveDebug->current.enabled)
			{
				if (recvCommands[i].type != RADIANT_COMMAND_CAMERA)
					Com_Printf(1, "Command %d [ID %d]:\n%s\n", recvCommands[i].type, recvCommands[i].liveUpdateId, recvCommands[i].strCommand);
			}

			memcpy(&gCommands[gCommandCount++], &recvCommands[i], sizeof(RadiantCommand));
		}
	}
	Sys_LeaveCriticalSection((CriticalSection)61);
}

bool RadiantRemoteUpdateSocket()
{
	if (g_ClientSocket != INVALID_SOCKET)
		return true;

	if (g_ServerSocket == INVALID_SOCKET)
		return false;

	// Check if there's a pending client connection request
	fd_set readSet;
	FD_ZERO(&readSet);
	FD_SET(g_ServerSocket, &readSet);

	// Zero timeout (poll)
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	int status = select(g_ServerSocket, &readSet, nullptr, nullptr, &timeout);

	if (status == SOCKET_ERROR)
		Com_Error(ERR_FATAL, "LiveRadiant: Failed to query socket status\n");

	// Must be 1 (handle) if there is a pending connection
	if (status != 1)
		return false;

	g_ClientSocket = accept(g_ServerSocket, nullptr, nullptr);

	if (g_ClientSocket == INVALID_SOCKET)
		Com_Error(ERR_FATAL, "LiveRadiant: Failed to accept a connection?\n");

	// Set non-blocking flag
	u_long socketMode = 1;
	ioctlsocket(g_ClientSocket, FIONBIO, &socketMode);

	Com_Printf(1, "LiveRadiant: Client connected\n");
	return true;
}

void G_AssignGameIdMapping(int liveUpdateId, int gameId)
{
	if (liveUpdateId > 0)
		gObjectMapping[liveUpdateId - 1].gameId = gameId;
}

int G_GetGameIdMapping(int liveUpdateId)
{
	if (liveUpdateId <= 0)
		return -1;

	return gObjectMapping[liveUpdateId - 1].gameId;
}

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

void G_ProcessPathnodeCommand(RadiantCommand *command, SpawnVar *spawnVar)
{
	const char *classname = GetPairValue(spawnVar, "classname");
	nodeType nodetype = G_GetNodeTypeFromClassname(classname);

	RadiantCommandType commandType = command->type;
	int gameId = G_GetGameIdMapping(command->liveUpdateId);

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
		// Remove all node links and then set it to invalid status
		g_radiant_selected_pathnode = G_FindPathNode(spawnVar, nodetype, gameId);

		if (g_radiant_selected_pathnode)
		{
			int nodeindex = Path_ConvertNodeToIndex(g_radiant_selected_pathnode);

			for (unsigned int i = 0; i < gameWorldCurrent->path.nodeCount; ++i)
			{
				pathnode_t *othernode = &gameWorldCurrent->path.nodes[i];

				for (unsigned short j = 0; j < othernode->constant.totalLinkCount; ++j)
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
		pathnode_t *node = 0;

		if (commandType)
		{
			if (commandType == RADIANT_COMMAND_UPDATE_SELECTED)
			{
				node = g_radiant_selected_pathnode;
			}
			else if (commandType == RADIANT_COMMAND_UPDATE)
			{
				node = G_FindPathNode(spawnVar, nodetype, gameId);
			}
		}
		else
		{
			node = g_radiant_selected_pathnode = G_FindPathNode(spawnVar, nodetype, gameId);

			if (!node)
				Com_Printf(5, "Radiant Live Update: Can't find pathnode. Maps out of sync (Radiant/Game), re-bsp!\n");
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

void G_ClearSelectedPathNode()
{
	g_radiant_selected_pathnode = nullptr;
}