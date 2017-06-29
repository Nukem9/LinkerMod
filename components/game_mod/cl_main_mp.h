#pragma once

typedef int DObj;
typedef int XAnimTree_s;
typedef int XAnim_s;

struct ViewModelHand
{
	int iPrevAnim;
	int iHandAnimIndex;
};
STATIC_ASSERT_OFFSET(ViewModelHand, iPrevAnim, 0x0);
STATIC_ASSERT_OFFSET(ViewModelHand, iHandAnimIndex, 0x4);

struct ViewModelInfo
{
	DObj *viewModelDObj;
	int hasAnimTree;
	XAnimTree_s *tree;
	XAnim_s *anims;
	unsigned int partBits[5];
	ViewModelHand hand[2];
};
STATIC_ASSERT_OFFSET(ViewModelInfo, anims, 0xC);
STATIC_ASSERT_OFFSET(ViewModelInfo, hand, 0x24);

struct  SnapshotInfo_s
{
	int clientNum;
	clientHeader_t *client;
	int snapshotDeltaTime;
	int unk;
	BYTE pad[0x1C];
};
STATIC_ASSERT_SIZE(SnapshotInfo_s, 0x2C);


VANILLA_VALUE(localClientConnectionState, int, 0x02910164);

static char* clc_demoName = (char*)0x028F9F3C;
static char* cls_servername = (char*)0x02B10B64;

VANILLA_VALUE(clc_demofile, int, 0x028F9F90);
VANILLA_VALUE(clc_demoplaying, int, 0x028F9F80);
VANILLA_VALUE(clc_isTimeDemo, int, 0x028F9F84);
VANILLA_VALUE(clc_lastClientArchiveIndex, int, 0x028F9F38);
VANILLA_VALUE(clc_firstDemoFrameSkipped, int, 0x028F9F8C);
VANILLA_VALUE(clc_demoPrevServerTime, int, 0x028F9F98);

VANILLA_VALUE(clc_demorecording, int, 0x028F9F7C);
VANILLA_VALUE(clc_demoLiveStream, int, 0x028F9F94);
VANILLA_VALUE(clc_demowaiting, int, 0x028F9F88);
 
VANILLA_VALUE(clc_reliableSequence, int, 0x028D9F20);
VANILLA_VALUE(clc_serverCommandSequence, int, 0x028E9F2C);

VANILLA_VALUE(clc_clientNum, int, 0x028D9DF4);
VANILLA_VALUE(clc_checksumFeed, int, 0x028D9F1C);
VANILLA_VALUE(clc_serverMessageSequence, int, 0x028E9F28);

VANILLA_FUNC(CL_ReadDemoMessage, void(__cdecl*)(int localClientNum), 0x00621560);
VANILLA_FUNC(CL_AllocatePerLocalClientMemory, void(__cdecl*)(const char *mapname, unsigned int flags), 0x004B73A0);
VANILLA_FUNC(CL_Disconnect, void(__cdecl*)(int localClientNum, bool deactivateClient), 0x005FEC50);

VANILLA_FUNC(CL_WriteAllDemoClientArchive, void (__cdecl*)(void), 0x00882560);

extern void (__cdecl * CL_Vid_Restart_Complete_f)();

void hk_CL_Vid_Restart_Complete_f();
void CL_CM_LoadMap(const char *mapname);

typedef const char *(__cdecl* CL_GetServerIPAddress_t)();
extern CL_GetServerIPAddress_t CL_GetServerIPAddress_o;

const char *__cdecl CL_GetServerIPAddress();

char *CG_GetLocalClientGlobals(int localClientNum);
ViewModelInfo *CG_GetLocalClientViewModelInfo(int localClientNum);

void __cdecl CL_WriteUncompressedDemoInfo(int localClientNum);
