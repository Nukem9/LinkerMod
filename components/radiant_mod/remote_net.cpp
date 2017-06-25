#include "stdafx.h"

SOCKET g_RemoteSocket;

void RemoteNet_SendSelect(const char *KVCommand)
{
	RadiantCommand cmd;
	memset(&cmd, 0, sizeof(RadiantCommand));

	cmd.type = RADIANT_COMMAND_SELECT;
	strcpy_s(cmd.strCommand, KVCommand);

	RemoteNet_SendPacket(&cmd);
}

void RemoteNet_SendUpdate(const char *KVCommand)
{
	RadiantCommand cmd;
	memset(&cmd, 0, sizeof(RadiantCommand));

	cmd.type = RADIANT_COMMAND_UPDATE_SELECTED;
	strcpy_s(cmd.strCommand, KVCommand);

	RemoteNet_SendPacket(&cmd);
}

void RemoteNet_UpdateCamera(float *Origin, float *Angles)
{
	RadiantCommand cmd;
	memset(&cmd, 0, sizeof(RadiantCommand));

	cmd.type = RADIANT_COMMAND_CAMERA;

	sprintf_s(cmd.strCommand, "{\n\"origin\" \"%.4f %.4f %.4f\"\n\"angles\" \"%.4f %.4f %.4f\"\n}",
		Origin[0], Origin[1], Origin[2],
		Angles[0], Angles[1], Angles[2]);

	RemoteNet_SendPacket(&cmd);
}

void RemoteNet_SendPacket(RadiantCommand *Command)
{
	if (g_RemoteSocket == INVALID_SOCKET)
		return;

	int ret = send(g_RemoteSocket, (const char *)Command, sizeof(RadiantCommand), 0);

	ASSERT(ret != SOCKET_ERROR);

	if (ret == SOCKET_ERROR)
	{
		closesocket(g_RemoteSocket);
		g_RemoteSocket = INVALID_SOCKET;
	}
}

DWORD WINAPI RemoteNet_Thread(LPVOID)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
		ASSERT_MSG(false, "WSAStartup ERROR!");

	while (true)
	{
		// Connect to the remote game TCP server
		g_RemoteSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (g_RemoteSocket == INVALID_SOCKET)
		{
			printf("Failed to initialize client TCP socket\n");
			return 0;
		}

		// Loop indefinitely until we successfully connect
		while (true)
		{
			sockaddr_in remoteAddr;
			remoteAddr.sin_family = AF_INET;
			remoteAddr.sin_port = htons(3700);
			remoteAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

			if (connect(g_RemoteSocket, (sockaddr *)&remoteAddr, sizeof(sockaddr)) != SOCKET_ERROR)
				break;

			Sleep(200);
		}

		// Send updates until the game exits/connection is terminated
		printf("NOTICE: Game connected\n");

		while (true)
		{
			// Check for a socket error
			if (g_RemoteSocket == INVALID_SOCKET)
				break;

			int error = 0;
			int len = sizeof(int);
			int retval = getsockopt(g_RemoteSocket, SOL_SOCKET, SO_ERROR, (char *)&error, &len);

			if (retval == SOCKET_ERROR || error != 0)
				break;

			// Send the camera packet every few milliseconds
			if (CCamWnd::ActiveWindow)
				RemoteNet_UpdateCamera(CCamWnd::ActiveWindow->cameraOrigin, CCamWnd::ActiveWindow->cameraAngles);

			Sleep(50);
		}

		printf("NOTICE: Game disconnected\n");
		closesocket(g_RemoteSocket);
	}

	return 0;
}