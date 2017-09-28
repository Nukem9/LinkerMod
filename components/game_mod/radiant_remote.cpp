#include "stdafx.h"

SOCKET g_ServerSocket = INVALID_SOCKET;
SOCKET g_ClientSocket = INVALID_SOCKET;

// /qcommon/radiant_remote.cpp:16
const char *GetPairValue(SpawnVar *spawnVar, const char *key)
{
	for (int i = 0; i < spawnVar->numSpawnVars; i++)
	{
		if (!_stricmp(key, spawnVar->spawnVars[i][0]))
			return spawnVar->spawnVars[i][1];
	}

	return nullptr;
}

// /qcommon/radiant_remote.cpp:176
void G_AssignGameIdMapping(int liveUpdateId, int gameId)
{
	if (liveUpdateId > 0)
		gObjectMapping[liveUpdateId - 1].gameId = gameId;
}

// /qcommon/radiant_remote.cpp:182
int G_GetGameIdMapping(int liveUpdateId)
{
	if (liveUpdateId <= 0)
		return -1;

	return gObjectMapping[liveUpdateId - 1].gameId;
}

// /qcommon/radiant_remote.cpp:???
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

// /qcommon/radiant_remote.cpp:???
void RadiantRemoteShutdown()
{
	shutdown(g_ServerSocket, 2 /*SD_BOTH*/);
	closesocket(g_ServerSocket);

	shutdown(g_ClientSocket, 2 /*SD_BOTH*/);
	closesocket(g_ClientSocket);

	g_ServerSocket = INVALID_SOCKET;
	g_ClientSocket = INVALID_SOCKET;
}

// /qcommon/radiant_remote.cpp:???
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

// /qcommon/radiant_remote.cpp:???
bool RadiantRemoteUpdateSocket()
{
	// Skip this function if client already connected
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

	// Must be 1 (handle) if someone is waiting
	if (status != 1)
		return false;

	g_ClientSocket = accept(g_ServerSocket, nullptr, nullptr);

	if (g_ClientSocket == INVALID_SOCKET)
		Com_Error(ERR_FATAL, "LiveRadiant: Failed to accept a connection!?\n");

	// Set non-blocking flag
	u_long socketMode = 1;
	ioctlsocket(g_ClientSocket, FIONBIO, &socketMode);

	Com_Printf(1, "LiveRadiant: Client connected\n");
	return true;
}