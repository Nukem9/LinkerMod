#pragma once

struct pathnode_tree_t;
struct pathnode_t;

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

struct pathlink_s
{
	float fDist;
	unsigned __int16 nodeNum;
	char disconnectCount;
	char negotiationLink;
	char ubBadPlaceCount[4];
};

struct pathstatic_t
{
	pathlink_s *pathLinks;
	unsigned __int16 *indirectNodes;
	char *pathbuf;
};

/* 1371 */
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

/* 1372 */
struct SentientHandle
{
	unsigned __int16 number;
	unsigned __int16 infoIndex;
};

/* 1373 */
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

/* 1375 */
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

/* 1374 */
struct pathnode_t
{
	pathnode_constant_t constant;
	pathnode_dynamic_t dynamic;
	pathnode_transient_t transient;
};

struct pathbasenode_t
{
	float vOrigin[3];
	unsigned int type;
};

/* 2836 */
struct pathnode_tree_nodes_t
{
	int nodeCount;
	unsigned __int16 *nodes;
};

/* 2837 */
union pathnode_tree_info_t
{
	pathnode_tree_t *child[2];
	pathnode_tree_nodes_t s;
};

/* 2835 */
struct pathnode_tree_t
{
	int axis;
	float dist;
	pathnode_tree_info_t u;
};

/* 2838 */
struct PathData
{
	unsigned int nodeCount;
	pathnode_t *nodes;
	pathbasenode_t *basenodes;
	unsigned int chainNodeCount;
	unsigned __int16 *chainNodeForNode;
	unsigned __int16 *nodeForChainNode;
	int visBytes;
	char *pathVis;
	int nodeTreeCount;
	pathnode_tree_t *nodeTree;
};

void Path_ErasePaths(const char* pathsError);

bool Path_LoadPathsInternal();
void Path_LoadPaths();

void Path_MakePathDataPermanent(pathlink_s *pSourceLinks, unsigned int totalLinkCount, char *pVis, int visBytes);
void Path_BuildNodeVis(char *visbuf, int iVisCacheSize);
void GetNodeAttackPoint(pathnode_t *node, float *attackPos);
pathnode_tree_t *Path_BuildNodeBsp_r(unsigned __int16 *nodes, int numNodes);
int Path_CheckExpandedNodeVis(int(*iNodeNums)[8192], int *iNodeCount, char *visbuf);
int Path_GetExpandedNodeNums_r(int iNodeStart, float fDistLeft, int *iNodeNums, int iNodeCount);

pathnode_t *Path_ConvertIndexToNode(int index);
int Path_IsCoverNode(pathnode_t *node);
int Path_IsValidClaimNode(pathnode_t *node);
pathnode_tree_t *Path_AllocPathnodeTree();
const int NodeVisCacheEntry_0(int i, int j);
const int ExpandedNodeVisCacheEntry_0(int i, int j);
bool Path_IsNegotiationLink(pathnode_t *pNodeFrom, pathnode_t *pNodeTo);