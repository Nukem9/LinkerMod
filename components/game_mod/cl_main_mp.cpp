#include "stdafx.h"
#include "com_constantconfigstrings.h"

char* szServerIPAddress = (char*)0x02EFCDA8;

void (__cdecl * CL_Vid_Restart_Complete_f)();

void hk_CL_Vid_Restart_Complete_f()
{
	// If the server is currently running, kill it first
	if (com_sv_running->current.enabled)
		SV_KillServer_f();

	CL_Vid_Restart_Complete_f();
}

void CL_CM_LoadMap(const char *mapname)
{
	((void(__cdecl *)(const char *))0x0056C4B0)(mapname);
}

CL_GetServerIPAddress_t CL_GetServerIPAddress_o = 0x0;

const char *__cdecl CL_GetServerIPAddress()
{
	if (cg_showServerInfo->current.enabled)
		return CL_GetServerIPAddress_o();

	return "";
}

char *CG_GetLocalClientGlobals(int localClientNum)
{
	ASSERT(localClientNum == 0);

	// cgArray
	return *(char **)0x02FF5354;
}

ViewModelInfo *CG_GetLocalClientViewModelInfo(int localClientNum)
{
	ASSERT(localClientNum == 0);

	// cg_viewModelArray
	return *(ViewModelInfo **)0x00C1C6D8;
}

typedef void(__cdecl* WriteGameState_f)(msg_t *msg);
static WriteGameState_f DynEnt_WriteGameState = (WriteGameState_f)0x0068E2B0;
static WriteGameState_f Destructible_WriteGameState = (WriteGameState_f)0x00641AF0;
static WriteGameState_f Rope_WriteGameState = (WriteGameState_f)0x00401DD0;

void __cdecl CL_WriteInitialStateToDemo(int demoFile, char type, int serverMessageSequence, char *compressedBuf, int compressedBufSize)
{
	FS_WriteToDemo(&type, 1, demoFile);
	int len = serverMessageSequence;
	FS_WriteToDemo(&len, 4, demoFile);
	len = compressedBufSize;
	FS_WriteToDemo(&len, 4, demoFile);
	for (len = 0; len < compressedBufSize; len += 1024)
	{
		int writeSize = (compressedBufSize - len > 1024) ? 1024 : compressedBufSize - len;
		FS_WriteToDemo(&compressedBuf[len], writeSize, demoFile);
	}
}

void __cdecl CL_WriteUncompressedDemoInfo(int localClientNum)
{
	char bufData[0x10000];
	msg_t buf;

	MSG_Init(&buf, bufData, 0x10000);
	//MSG_ClearLastReferencedEntity(&buf);

	// These two arent actually visible in CL_ParseGameState but theyre there
	MSG_WriteLong(&buf, clc_reliableSequence);

	//
	// Server metadata
	//
	MSG_WriteByte(&buf, 1); // message chunk header
	MSG_WriteLong(&buf, clc_serverCommandSequence);
	const char* mapname = Dvar_GetString("mapname");
	MSG_WriteString(&buf, mapname);
	const char* gametype = Dvar_GetString("g_gametype");
	MSG_WriteString(&buf, gametype);

	// Script checksums
	int scr_checksum[3];
	Scr_GetChecksumNoAsserts(scr_checksum, SCRIPTINSTANCE_SERVER);
	MSG_WriteLong(&buf, scr_checksum[0]);
	MSG_WriteLong(&buf, scr_checksum[1]);
	MSG_WriteLong(&buf, scr_checksum[2]);

	// Used to debug where the configstring checksum is
	//for (int i = 0; i < 64 ; i++)
	//	MSG_WriteLong(&buf, i + 1);

#if CCS_ALLOW_CHECKSUM
	int checksum = CCS_GetChecksum();
#else
	int checksum = NULL;
#endif
	MSG_WriteLong(&buf, checksum);

	//
	// Start the configstring message chunk
	//
	MSG_WriteByte(&buf, 2);
	
	// Count the number of config strings
	int configStringCount = 0;
	for (int i = 0; i < 3112; i++)
	{
		if (reinterpret_cast<int*>(0x02EE9CF0)[i])
			configStringCount++;
	}

	MSG_WriteShort(&buf, configStringCount);

	// Write each config string
	for (int i = 0; i < 3112; i++)
	{
		int v = reinterpret_cast<int*>(0x02EE9CEC)[i];
		if (v)
		{
			char* s = (char*)(0x02EECD8C + v);
			MSG_WriteBit0(&buf);
			MSG_WriteBits(&buf, i, 12);
			MSG_WriteBigString(&buf, s);
		}
	}

#if 1 //_EXPERIMENTAL
	//
	// Entities
	//
	DWORD* unk_old = (DWORD*)0x02B10CCC;
	DWORD* unk_new = (DWORD*)0x028C4F70;
	unk_new[0] = unk_old[0];
	unk_new[1] = unk_old[1];
	unk_new[2] = unk_old[2];

	SnapshotInfo_s snapInfo;
	char nullState[0xD8];

	memset(&snapInfo, 0, sizeof(snapInfo));
	memset(nullState, 0, ARRAYSIZE(nullState));

	for (int i = 0; i < 400; i++)
	{
		int* ent = reinterpret_cast<int*>(0x027F9710) + (i * 0x168);
		//int* ent = reinterpret_cast<int*>(0x029B0E98) + (i * 0x36);
		if (*ent)
		{
			MSG_WriteByte(&buf, 4);
			snapInfo.clientNum = clc_clientNum;
			snapInfo.snapshotDeltaTime = -1;
			snapInfo.unk = 2;
			MSG_WriteEntity(&snapInfo, &buf, 0, (entityState_s*)&nullState, (entityState_s*)ent, 4096);
			snapInfo.unk = 0;
		}
	}

	MSG_ClearLastReferencedEntity(&buf);

	for (int i = 0; i < 350; i++)
	{
		int* ent = reinterpret_cast<int*>(0x027F9710 + 0x59FF8) + (i * 0x36);
		//int* ent = reinterpret_cast<int*>(0x029E6E98) + (i * 0x36);
		if (*ent)
		{
			MSG_WriteByte(&buf, 5);
			snapInfo.clientNum = clc_clientNum;
			snapInfo.snapshotDeltaTime = -1;
			snapInfo.unk = 2;
			MSG_WriteEntity(&snapInfo, &buf, 0, (entityState_s*)&nullState, (entityState_s*)ent, 4096);
			snapInfo.unk = 0;
		}
	}
#endif

	//
	// Additional gamestate data
	//
	MSG_WriteByte(&buf, 14);
	MSG_WriteLong(&buf, clc_clientNum);
	MSG_WriteLong(&buf, clc_checksumFeed);
	MSG_WriteByte(&buf, 6);
	DynEnt_WriteGameState(&buf);
	MSG_WriteByte(&buf, 7);
	Destructible_WriteGameState(&buf);
	MSG_WriteByte(&buf, 8);
	Rope_WriteGameState(&buf);
	MSG_WriteByte(&buf, 14);

	//
	// Compress the data & save it
	//
	char compressedBuf[65536];
	*(DWORD*)compressedBuf = *(DWORD*)buf.data;
	int compressedSize = MSG_WriteBitsCompress(0, (const char *)buf.data + 4, buf.cursize - 4, compressedBuf + 4, 65532) + 4;
	CL_WriteInitialStateToDemo(clc_demofile, 0, clc_serverMessageSequence, (char *)compressedBuf, compressedSize);
	CL_WriteAllDemoClientArchive();

	clc_lastClientArchiveIndex = *(int*)0x02996854;
}