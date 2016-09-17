#include "stdafx.h"

HANDLE liveUpdateThread;

int& savedCommandCount		= *(int *)0x0251AE58;
auto savedCommands			= (RadiantCommand *)0x02517D90;
int& gCommandCount			= *(int *)0x251AE50;
auto gCommands				= (RadiantCommand *)0x02517D90;
int& gObjectMappingCount	= *(int *)0x0251AE60;
auto gObjectMapping			= (RadaintToGameMapping *)0x2507180;

void RadiantRemoteInit()
{
	savedCommandCount = 0;
	memset(savedCommands, 0, sizeof(RadiantCommand) * 128);

	gCommandCount = 0;
	memset(gCommands, 0, sizeof(RadiantCommand) * 24);

	gObjectMappingCount = 0;
	memset(gObjectMapping, 0, sizeof(RadaintToGameMapping) * 128);

	if (!liveUpdateThread)
		liveUpdateThread = CreateThread(nullptr, 0, RadiantRemoteThread, nullptr, 0, nullptr);
}

DWORD WINAPI RadiantRemoteThread(LPVOID Arg)
{
	// Initialize winsock if the game hasn't yet
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		Com_PrintError(1, "LiveRadiant: WSAStartup failed\n");
		return 0;
	}

	// Create a UDP socket
	SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (udpSocket == INVALID_SOCKET)
	{
		Com_PrintError(1, "LiveRadiant: Socket creation failed\n");
		return 0;
	}

	// Bind socket to any incoming address on port 3700
	sockaddr_in addrSender;
	sockaddr_in addrIn;
	addrIn.sin_family		= AF_INET;
	addrIn.sin_port			= htons(3700);
	addrIn.sin_addr.s_addr	= htonl(INADDR_ANY);

	if (bind(udpSocket, (SOCKADDR *)&addrIn, sizeof(addrIn)) == SOCKET_ERROR)
	{
		Com_PrintError(1, "LiveRadiant: Failed to bind socket\n");
		closesocket(udpSocket);
		return 0;
	}

	Com_Printf(1, "LiveRadiant: Socket bound on port 3700\n");

	while (true)
	{
		char recvBuf[2048];
		memset(recvBuf, 0, sizeof(recvBuf));

		int fromSize = sizeof(sockaddr_in);
		int recvSize = recvfrom(udpSocket, recvBuf, sizeof(recvBuf), 0, (SOCKADDR *)&addrSender, &fromSize);

		if (recvSize == SOCKET_ERROR)
		{
			Com_PrintError(1, "LiveRadiant: Socket receive from failed\n");
			break;
		}

		// Data received from network, now tell the game
		Sys_EnterCriticalSection((CriticalSection)61);
		{
			RadiantCommand *c = (RadiantCommand *)&recvBuf;

			if (c->type != RADIANT_COMMAND_CAMERA)
				Com_Printf(1, "Command %d %d:\n%s\n", c->type, c->liveUpdateId, c->strCommand);

			// Insert in global array
			memcpy(&gCommands[gCommandCount++], c, sizeof(RadiantCommand));
		}
		Sys_LeaveCriticalSection((CriticalSection)61);
	}

	shutdown(udpSocket, 2 /*SD_BOTH*/);
	closesocket(udpSocket);
	return 0;
}