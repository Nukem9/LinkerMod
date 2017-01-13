#pragma once

enum RadiantCommandType
{
	RADIANT_COMMAND_SELECT = 0x0,
	RADIANT_COMMAND_DESELECT = 0x1,
	RADIANT_COMMAND_UPDATE_SELECTED = 0x2,
	RADIANT_COMMAND_UPDATE = 0x3,
	RADIANT_COMMAND_CREATE = 0x4,
	RADIANT_COMMAND_DELETE = 0x5,
	RADIANT_COMMAND_CAMERA = 0x6,
};

struct RadiantCommand
{
	RadiantCommandType type;
	int liveUpdateId;
	char strCommand[512];
};

void RemoteNet_SendSelect(const char *KVCommand);
void RemoteNet_SendUpdate(const char *KVCommand);
void RemoteNet_UpdateCamera(float *Origin, float *Angles);
void RemoteNet_SendPacket(RadiantCommand *Command);
DWORD WINAPI RemoteNet_Thread(LPVOID);