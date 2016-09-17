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

struct RadaintToGameMapping
{
	int fromRadiantId;
	int liveUpdateId;
	int gameId;
	int cg_gameId;
};

void RadiantRemoteInit();
DWORD WINAPI RadiantRemoteThread(LPVOID Arg);