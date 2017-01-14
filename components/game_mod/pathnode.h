#pragma once

struct SpawnVar
{
	bool spawnVarsValid;
	int numSpawnVars;
	char *spawnVars[64][2];
	int numSpawnVarChars;
	char spawnVarChars[2048];
};
STATIC_ASSERT_OFFSET(SpawnVar, numSpawnVars, 0x4);
STATIC_ASSERT_OFFSET(SpawnVar, spawnVars, 0x8);

struct SentientHandle
{
	unsigned __int16 number;
	unsigned __int16 infoIndex;
};

#define MAX_PATH_DYNAMIC_NODES	128
#define MAX_SPAWN_EXIT_NODES	100
#define MAX_NODES_IN_BRUSH		512

#define PATH_MAX_NODES			8192
#define PATH_MAX_TOTAL_LINKS	524288
#define MAX_BADPLACES			256

enum nodeType
{
	NODE_BADNODE = 0x0,
	NODE_PATHNODE = 0x1,
	NODE_COVER_STAND = 0x2,
	NODE_COVER_CROUCH = 0x3,
	NODE_COVER_CROUCH_WINDOW = 0x4,
	NODE_COVER_PRONE = 0x5,
	NODE_COVER_RIGHT = 0x6,
	NODE_COVER_LEFT = 0x7,
	NODE_COVER_WIDE_RIGHT = 0x8,
	NODE_COVER_WIDE_LEFT = 0x9,
	NODE_COVER_PILLAR = 0xA,
	NODE_CONCEALMENT_STAND = 0xB,
	NODE_CONCEALMENT_CROUCH = 0xC,
	NODE_CONCEALMENT_PRONE = 0xD,
	NODE_REACQUIRE = 0xE,
	NODE_BALCONY = 0xF,
	NODE_SCRIPTED = 0x10,
	NODE_NEGOTIATION_BEGIN = 0x11,
	NODE_NEGOTIATION_END = 0x12,
	NODE_TURRET = 0x13,
	NODE_GUARD = 0x14,
	NODE_NUMTYPES = 0x15,
	NODE_DONTLINK = 0x15,
};

struct pathlink_s;
struct pathnode_t;

struct pathlink_s
{
	float fDist;						// this+0x0
	unsigned short nodeNum;				// this+0x4
	unsigned char disconnectCount;		// this+0x6
	unsigned char negotiationLink;		// this+0x7
	unsigned char ubBadPlaceCount[4];	// this+0x8
};
STATIC_ASSERT_SIZE(pathlink_s, 0xC);

struct pathnode_constant_t
{
	nodeType type;
	unsigned __int16 spawnflags;
	unsigned __int16 targetname;
	unsigned __int16 script_linkName;
	unsigned __int16 script_noteworthy;
	unsigned __int16 target;
	unsigned __int16 animscript;
	int animscriptfunc;
	float vOrigin[3];
	float fAngle;
	float forward[2];
	float fRadius;
	float minUseDistSq;
	__int16 wOverlapNode[2];
	__int16 wChainId;
	__int16 wChainDepth;
	__int16 wChainParent;
	unsigned __int16 totalLinkCount;
	pathlink_s *Links;
};

struct pathnode_dynamic_t
{
	SentientHandle pOwner;
	int iFreeTime;
	int iValidTime[3];
	int inPlayerLOSTime;
	__int16 wLinkCount;
	__int16 wOverlapCount;
	__int16 turretEntNumber;
	__int16 userCount;
};

struct pathnode_transient_t
{
	int iSearchFrame;
	pathnode_t *pNextOpen;
	pathnode_t *pPrevOpen;
	pathnode_t *pParent;
	float fCost;
	float fHeuristic;
	float costFactor;
};

struct pathnode_t
{
	pathnode_constant_t constant;
	pathnode_dynamic_t dynamic;
	pathnode_transient_t transient;
};
STATIC_ASSERT_SIZE(pathnode_t, 0x80);

struct pathbasenode_t
{
	float vOrigin[3];
	unsigned int type;
};
STATIC_ASSERT_OFFSET(pathbasenode_t, vOrigin, 0x0);
STATIC_ASSERT_OFFSET(pathbasenode_t, type, 0xC);

struct pathnode_tree_t
{
	char _pad[0x1];
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

void node_droptofloor(pathnode_t *node);
void G_InitPathBaseNode(pathbasenode_t *pbnode, pathnode_t *pnode);
void G_DropPathNodeToFloor(unsigned int nodeIndex);
void G_SpawnPathnodeDynamic(SpawnVar *spawnVar);
unsigned int Path_ConvertNodeToIndex(pathnode_t *node);
void Path_Init(int restart);
void Path_InitBadPlaces();
void Path_InitLinkCounts();
void Path_InitLinkInfoArray();
void Path_InitPaths();
void Path_CheckSpawnExitNodesConnectivity();
void Path_ValidateNode(pathnode_t *node);
void Path_ValidateAllNodes();

typedef struct
{
	const char *name;
	PathData path;
} GameWorldMp, GameWorldSp;
STATIC_ASSERT_SIZE(GameWorldMp, 0x2C);

static GameWorldMp*& gameWorldCurrent = *(GameWorldMp **)0x00B77734;