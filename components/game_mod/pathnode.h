#pragma once

#define MAX_PATH_DYNAMIC_NODES	128
#define MAX_SPAWN_EXIT_NODES	100
#define MAX_NODES_IN_BRUSH		512

#define PATH_MAX_NODES			8192
#define PATH_MAX_TOTAL_LINKS	524288
#define MAX_BADPLACES			256

struct pathnode_t
{
	char _pad[0x80];
};
STATIC_ASSERT_SIZE(pathnode_t, 0x80);

struct pathbasenode_t
{
	char _pad[0x1];
};

struct pathnode_tree_t
{
	char _pad[0x1];
};

struct pathlink_s
{
	float fDist;						// this+0x0
	unsigned short nodeNum;				// this+0x4
	unsigned char disconnectCount;		// this+0x6
	unsigned char negotiationLink;		// this+0x7
	unsigned char ubBadPlaceCount[4];	// this+0x8
};

struct badplace_t
{
	char _pad[0x2C];
};
STATIC_ASSERT_SIZE(badplace_t, 0x2C);

struct path_t
{
	char _pad[0x1];
};

struct pathstatic_t
{
	pathlink_s *pathLinks;			// this+0x0
	unsigned short *indirectNodes;	// this+0x4
	unsigned char *pathbuf;			// this+0x8
};
STATIC_ASSERT_SIZE(pathstatic_t, 0xC);

struct pathlocal_t
{
	char _pad[0x4080];
};
STATIC_ASSERT_SIZE(pathlocal_t, 0x4080);

struct PathData
{
	unsigned int nodeCount;				// this+0x0
	pathnode_t *nodes;					// this+0x4
	pathbasenode_t *basenodes;			// this+0x8
	unsigned int chainNodeCount;		// this+0xC
	unsigned short *chainNodeForNode;	// this+0x10
	unsigned short *nodeForChainNode;	// this+0x14
	int visBytes;						// this+0x18
	unsigned char *pathVis;				// this+0x1C
	int nodeTreeCount;					// this+0x20
	pathnode_tree_t *nodeTree;			// this+0x24
};
STATIC_ASSERT_SIZE(PathData, 0x28);

static pathlocal_t *g_path = (pathlocal_t *)0x01D00400;
static const char*& g_pathsError = *(const char **)0x01D0496C;
static badplace_t *g_badplaces = (badplace_t *)0x01A4C088;
static path_t*& debugPath = *(path_t **)0x01D04484;
static pathstatic_t *pathstatic = (pathstatic_t *)0x01D04970;

void Path_Init(int restart);
void Path_CreateNodes();
void Path_InitStatic(int restart);
void Path_InitBadPlaces();
void Path_InitPaths();

typedef struct
{
	const char *name;
	PathData path;
} GameWorldMp, GameWorldSp;
STATIC_ASSERT_SIZE(GameWorldMp, 0x2C);

static GameWorldMp*& gameWorldCurrent = *(GameWorldMp **)0x00B77734;