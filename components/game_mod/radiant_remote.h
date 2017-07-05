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
STATIC_ASSERT_SIZE(RadiantCommand, 0x208);

struct RadaintToGameMapping
{
	int fromRadiantId;
	int liveUpdateId;
	int gameId;
	int cg_gameId;
};
STATIC_ASSERT_SIZE(RadaintToGameMapping, 0x10);

static int& savedCommandCount = *(int *)0x0251AE58;
static auto savedCommands = (RadiantCommand *)0x02507990;
static int& gCommandCount = *(int *)0x251AE50;
static auto gCommands = (RadiantCommand *)0x02517D90;
static int& gObjectMappingCount = *(int *)0x0251AE60;
static auto gObjectMapping = (RadaintToGameMapping *)0x2507180;

static pathnode_t*& g_radiant_selected_pathnode = *(pathnode_t **)0x01D04870;

void RadiantRemoteInit();
void RadiantRemoteShutdown();
void RadiantRemoteUpdate();
bool RadiantRemoteUpdateSocket();

void G_AssignGameIdMapping(int liveUpdateId, int gameId);
int G_GetGameIdMapping(int liveUpdateId);
pathnode_t *G_FindPathNode(SpawnVar *spawnVar, nodeType type, const int gameId);
void G_ProcessPathnodeCommand(RadiantCommand *command, SpawnVar *spawnVar);
void G_ClearSelectedPathNode();