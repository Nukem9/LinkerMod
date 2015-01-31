#include "stdafx.h"

bool Path_LoadPathsInternal()
{
	if (!GameWorldCurrent->path.nodeCount)
		return true;

	unsigned int len;
	const void *buf = Com_LoadBspLump(GameWorldCurrent->name, LUMP_PATHCONNECTIONS, 1, &len);

	if (len <= 0)
		return false;

	if (*(DWORD *)buf != 8)
	{
		Com_UnloadBspLump(LUMP_PATHCONNECTIONS);
		return false;
	}

	int pos = (int)((char *)buf + 6);

	if (GameWorldCurrent->path.nodeCount != *((WORD *)buf + 2))
		Com_Error(1, "Path node count mismatch in bsp file. Recompile the map to fix.");

	pathlink_s *linksBuffer = (pathlink_s *)Z_Malloc(0x600000);// "Path_LoadPaths"

	unsigned int totalLinkCount = 0;
	for (unsigned int i = 0; i < GameWorldCurrent->path.nodeCount; ++i)
	{
		pathnode_t *nodeFrom = &GameWorldCurrent->path.nodes[i];

		nodeFrom->constant.totalLinkCount = *(WORD *)pos;
		pos += 2;

		for (unsigned int j = 0; j < nodeFrom->constant.totalLinkCount; ++j)
		{
			pathlink_s *link = &linksBuffer[j + totalLinkCount];

			ASSERT(!j || link > &linksBuffer[totalLinkCount + (j - 1)]);

			WORD v2 = *(WORD *)pos;
			int v3 = pos + 2;

			link->nodeNum = v2;

			float v4 = *(float *)v3;
			pos = v3 + 4;

			link->fDist = v4;
			link->disconnectCount = 0;
			link->negotiationLink = Path_IsNegotiationLink(nodeFrom, &GameWorldCurrent->path.nodes[link->nodeNum]);
		}

		totalLinkCount += nodeFrom->constant.totalLinkCount;
	}

	int visBytes = (GameWorldCurrent->path.nodeCount * (GameWorldCurrent->path.nodeCount - 1) + 7) >> 3;
	void *visbuf = Z_Malloc(visBytes);// "Path_LoadPaths"

	int i = 0;
	while (i < visBytes)
	{
		if (*(BYTE *)pos & 0x80)
		{
			int ja = (unsigned __int8)~*(BYTE *)pos++;
			do
			{
				*((BYTE *)visbuf + i++) = *(BYTE *)pos++;
				--ja;
			} while (ja);
		}
		else
		{
			BYTE v5 = *(BYTE *)pos;
			int v6 = pos + 1;

			memset((char *)visbuf + i, 0, v5);

			i += v5;
			*((BYTE *)visbuf + i++) = *(BYTE *)v6;
			pos = v6 + 1;
		}
	}

	ASSERT(i == visBytes);

	Path_MakePathDataPermanent(linksBuffer, totalLinkCount, (char *)visbuf, visBytes);
	Z_Free(visbuf);
	Z_Free(linksBuffer);

	Com_UnloadBspLump(LUMP_PATHCONNECTIONS);
	return true;
}

void Path_LoadPaths()
{
	if (!Path_LoadPathsInternal())
		Path_ErasePaths("\n\nMap does not have paths generated.\nRun the game with \"g_connectpaths 1\" to generate paths.\nRun the game with \"g_connectpaths 2\" to generate paths and auto quit.\n");
}

void Path_MakePathDataPermanent(pathlink_s *pSourceLinks, unsigned int totalLinkCount, char *pVis, int visBytes)
{
	unsigned int totalBytes = sizeof(pathlink_s) * totalLinkCount;

	if (!pathstatic->pathLinks)
		pathstatic->pathLinks = (pathlink_s *)Hunk_Alloc(totalBytes);// "Path_MakeLinksPermanent"

	memcpy(pathstatic->pathLinks, pSourceLinks, totalBytes);

	for (unsigned int i = 0; i < totalLinkCount; ++i)
		ASSERT(pathstatic->pathLinks[i].negotiationLink == (pathstatic->pathLinks[i].negotiationLink != 0));

	int accountedLinks = 0;
	for (unsigned int i = 0; i < GameWorldCurrent->path.nodeCount; ++i)
	{
		if (GameWorldCurrent->path.nodes[i].constant.totalLinkCount)
		{
			GameWorldCurrent->path.nodes[i].constant.Links = &pathstatic->pathLinks[accountedLinks];
			accountedLinks += GameWorldCurrent->path.nodes[i].constant.totalLinkCount;
		}
	}

	ASSERT(accountedLinks == totalLinkCount);

	char *visbuf = nullptr;
	if (pVis)
	{
		ASSERT(pVis != nullptr);
		visbuf = 0;
	}
	else
	{
		int visBytes = (GameWorldCurrent->path.nodeCount * (GameWorldCurrent->path.nodeCount - 1) + 7) >> 3;
		visbuf = (char *)Z_Malloc(visBytes);// "Path_MakePathDataPermanent"

		Path_BuildNodeVis(visbuf, visBytes);
		pVis = visbuf;
	}

	if (!GameWorldCurrent->path.pathVis)
	{
		GameWorldCurrent->path.visBytes = visBytes;
		GameWorldCurrent->path.pathVis = (char *)Hunk_Alloc(visBytes);// "VisData"
	}

	memcpy(GameWorldCurrent->path.pathVis, pVis, visBytes);

	if (visbuf)
		Z_Free(visbuf);

	if (!pathstatic->indirectNodes)
	{
		pathstatic->indirectNodes = (unsigned __int16 *)Hunk_Alloc(2 * GameWorldCurrent->path.nodeCount);// "Path_MakeNodeBsp"

		for (unsigned int ib = 0; ib < GameWorldCurrent->path.nodeCount; ++ib)
			pathstatic->indirectNodes[ib] = ib;

		GameWorldCurrent->path.nodeTree = Path_BuildNodeBsp_r(pathstatic->indirectNodes, GameWorldCurrent->path.nodeCount);
	}
}

float vec3_origin[3] = { 0.0f, 0.0f, 0.0f };
void __cdecl Path_BuildNodeVis(char *visbuf, int iVisCacheSize)
{
	signed int v2; // eax@9
	signed int v3; // ST20_4@9
	int j; // [sp+Ch] [bp-44h]@6
	int ja; // [sp+Ch] [bp-44h]@16
	signed int entry; // [sp+10h] [bp-40h]@19
	int nodeCount[2]; // [sp+34h] [bp-1Ch]@16
	int i; // [sp+44h] [bp-Ch]@4

	Com_Printf(18, "Building node vis...\n");

	memset(visbuf, 0, iVisCacheSize);
	unsigned int totalCount = 2 * GameWorldCurrent->path.nodeCount;
	void *nodeNums = Hunk_AllocateTempMemory(65536);// "Path_BuildNodeVis"

	ASSERT(nodeNums != nullptr);

	int count = 0;
	int prevPercent = 0;
	for (i = 0; i < GameWorldCurrent->path.nodeCount; ++i)
	{
		float start[3];
		float end[3];

		GetNodeAttackPoint(&GameWorldCurrent->path.nodes[i], start);

		for (j = i + 1; j < GameWorldCurrent->path.nodeCount; ++j)
		{
			GetNodeAttackPoint(&GameWorldCurrent->path.nodes[j], end);
			int hitNum = CM_BoxSightTrace(0, start, end, vec3_origin, vec3_origin, 0, 0x2809803);

			if (!hitNum)
			{
				v2 = NodeVisCacheEntry_0(i, j);
				visbuf[v2 >> 3] |= 1 << (v2 & 7);
				v3 = ExpandedNodeVisCacheEntry_0(j, i);
				visbuf[v3 >> 3] |= 1 << (v3 & 7);
			}
		}

		++count;
		if (5 * 20 * count / (signed int)totalCount != prevPercent)
		{
			prevPercent = 5 * 20 * count / (signed int)totalCount;
			Com_Printf(18, "%d%%\n", prevPercent);
		}
	}

	for (i = 0; i < GameWorldCurrent->path.nodeCount; ++i)
	{
		*(DWORD *)nodeNums = i;
		nodeCount[0] = Path_GetExpandedNodeNums_r(i, 256.0, (int *)nodeNums, 1);
		for (ja = i + 1; ja < GameWorldCurrent->path.nodeCount; ++ja)
		{
			entry = ExpandedNodeVisCacheEntry_0(ja, i);
			if (!((1 << (entry & 7)) & (unsigned __int8)visbuf[entry >> 3]))
			{
				*((DWORD *)nodeNums + 8192) = ja;
				nodeCount[1] = Path_GetExpandedNodeNums_r(ja, 256.0, (int *)nodeNums + 8192, 1);
				if (Path_CheckExpandedNodeVis((int(*)[8192])nodeNums, nodeCount, visbuf))
					visbuf[entry >> 3] |= 1 << (entry & 7);
			}
		}
		++count;
		if (5 * 20 * count / (signed int)totalCount != prevPercent)
		{
			prevPercent = 5 * 20 * count / (signed int)totalCount;
			Com_Printf(18, "%d%%\n", prevPercent);
		}
	}

	Hunk_FreeTempMemory(nodeNums);
	Com_Printf(18, "Building node vis done.\n");
}

// const float nodeAttackOffset[21]
BYTE nodeAttackOffset[21 * 4] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xB4, 0xC2, 0x00, 0x00, 0xB4, 0x42, 0x00, 0x00, 0xB4, 0xC2,
	0x00, 0x00, 0xB4, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

float __real3 = 0.00;
float __real4 = 0.017453292;
float AI_LEAN_DIST = 44.0;
float ACTOR_EYE_OFFSET = 64.0;
__declspec(naked) void __cdecl GetNodeAttackPoint(pathnode_t *node, float *attackPos)
{
	__asm
	{
	PUSH EBP
	MOV EBP,ESP
	SUB ESP,0x1C
	CMP DWORD PTR SS:[EBP+0x8],0x0
	JNZ L016

	/*
	PUSH 0xCB6C40
	PUSH 0xC5C544
	PUSH 0x0
	PUSH 0x568
	PUSH 0xD39284
	CALL 0x0788720
	ADD ESP,0x14
	MOVZX EAX,AL
	TEST EAX,EAX
	JNZ L016
	*/
	INT 3 

	L016:
	MOV EDX,DWORD PTR SS:[EBP+0x8]
	PUSH EDX
	CALL Path_IsValidClaimNode
	ADD ESP,0x4
	TEST EAX,EAX
	JE L080
	MOV EAX,DWORD PTR SS:[EBP+0x8]
	PUSH EAX
	CALL Path_IsCoverNode
	ADD ESP,0x4
	TEST EAX,EAX
	JE L080
	MOV ECX,DWORD PTR SS:[EBP+0x8]
	MOV EDX,DWORD PTR DS:[ECX]
	MOVSS XMM0,DWORD PTR DS:[EDX*0x4+offset nodeAttackOffset] // nodeAttackOffset
	UCOMISS XMM0,DWORD PTR DS:[__real3]
	LAHF 
	TEST AH,0x44
	JPO L080
	MOV EAX,DWORD PTR SS:[EBP+0x8]
	MOVZX ECX,WORD PTR DS:[EAX+0x4]
	AND ECX,0x8000
	JE L079
	MOV EDX,DWORD PTR SS:[EBP+0x8]
	MOV EAX,DWORD PTR DS:[EDX]
	MOV ECX,DWORD PTR SS:[EBP+0x8]
	MOVSS XMM0,DWORD PTR DS:[ECX+0x20]
	ADDSS XMM0,DWORD PTR DS:[EAX*0x4+offset nodeAttackOffset] // nodeAttackOffset
	MOVSS DWORD PTR SS:[EBP-0xC],XMM0
	LEA EDX,DWORD PTR SS:[EBP-0x8]
	MOV DWORD PTR SS:[EBP-0x18],EDX
	LEA EAX,DWORD PTR SS:[EBP-0x4]
	MOV DWORD PTR SS:[EBP-0x14],EAX
	MOVSS XMM0,DWORD PTR SS:[EBP-0xC]
	MULSS XMM0,DWORD PTR DS:[__real4]
	MOVSS DWORD PTR SS:[EBP-0x10],XMM0
	PUSH EAX
	PUSH ECX
	FLD DWORD PTR SS:[EBP-0x10]
	FSINCOS 
	MOV ECX,DWORD PTR SS:[EBP-0x18]
	MOV EAX,DWORD PTR SS:[EBP-0x14]
	FSTP DWORD PTR DS:[ECX]
	FSTP DWORD PTR DS:[EAX]
	POP ECX
	POP EAX
	MOVSS XMM0,DWORD PTR DS:[AI_LEAN_DIST]
	MULSS XMM0,DWORD PTR SS:[EBP-0x8]
	MOV ECX,DWORD PTR SS:[EBP+0x8]
	ADDSS XMM0,DWORD PTR DS:[ECX+0x14]
	MOV EDX,DWORD PTR SS:[EBP+0xC]
	MOVSS DWORD PTR DS:[EDX],XMM0
	MOVSS XMM0,DWORD PTR DS:[AI_LEAN_DIST]
	MULSS XMM0,DWORD PTR SS:[EBP-0x4]
	MOV EAX,DWORD PTR SS:[EBP+0x8]
	ADDSS XMM0,DWORD PTR DS:[EAX+0x18]
	MOV ECX,DWORD PTR SS:[EBP+0xC]
	MOVSS DWORD PTR DS:[ECX+0x4],XMM0
	MOV EDX,DWORD PTR SS:[EBP+0x8]
	MOVSS XMM0,DWORD PTR DS:[EDX+0x1C]
	ADDSS XMM0,DWORD PTR DS:[ACTOR_EYE_OFFSET]
	MOV EAX,DWORD PTR SS:[EBP+0xC]
	MOVSS DWORD PTR DS:[EAX+0x8],XMM0
	L079:
	JMP L100
	L080:
	MOV ECX,DWORD PTR SS:[EBP+0x8]
	ADD ECX,0x14
	MOV DWORD PTR SS:[EBP-0x1C],ECX
	MOV EDX,DWORD PTR SS:[EBP+0xC]
	MOV EAX,DWORD PTR SS:[EBP-0x1C]
	FLD DWORD PTR DS:[EAX]
	FSTP DWORD PTR DS:[EDX]
	MOV ECX,DWORD PTR SS:[EBP+0xC]
	MOV EDX,DWORD PTR SS:[EBP-0x1C]
	FLD DWORD PTR DS:[EDX+0x4]
	FSTP DWORD PTR DS:[ECX+0x4]
	MOV EAX,DWORD PTR SS:[EBP+0xC]
	MOV ECX,DWORD PTR SS:[EBP-0x1C]
	FLD DWORD PTR DS:[ECX+0x8]
	FSTP DWORD PTR DS:[EAX+0x8]
	MOV EDX,DWORD PTR SS:[EBP+0xC]
	MOVSS XMM0,DWORD PTR DS:[EDX+0x8]
	ADDSS XMM0,DWORD PTR DS:[ACTOR_EYE_OFFSET]
	MOV EAX,DWORD PTR SS:[EBP+0xC]
	MOVSS DWORD PTR DS:[EAX+0x8],XMM0
	L100:
	MOV ESP,EBP
	POP EBP
	RETN 
	}
}

float __real1 = 192.000;
float __real2 = 0.5;
__declspec(naked) pathnode_tree_t * __cdecl Path_BuildNodeBsp_r(unsigned __int16 *nodes, int numNodes)
{
	__asm
	{
	L000:
	PUSH EBP
	MOV EBP,ESP
	SUB ESP,0x3C
	PUSH ESI
	CMP DWORD PTR SS:[EBP+0x0C],0x4
	JGE L018
	CALL Path_AllocPathnodeTree
	MOV DWORD PTR SS:[EBP-0x34],EAX
	MOV EAX,DWORD PTR SS:[EBP-0x34]
	MOV DWORD PTR DS:[EAX],-0x1
	MOV ECX,DWORD PTR SS:[EBP-0x34]
	MOV EDX,DWORD PTR SS:[EBP+0x0C]
	MOV DWORD PTR DS:[ECX+0x8],EDX
	MOV EAX,DWORD PTR SS:[EBP-0x34]
	MOV ECX,DWORD PTR SS:[EBP+0x8]
	MOV DWORD PTR DS:[EAX+0x0C],ECX
	MOV EAX,DWORD PTR SS:[EBP-0x34]
	JMP L298
	L018:
	MOV EDX,DWORD PTR SS:[EBP+0x8]
	MOVZX EAX,WORD PTR DS:[EDX]
	SHL EAX,0x7
	MOV ECX,DWORD PTR DS:[0x574A98]
	ADD EAX,DWORD PTR DS:[ECX+0x8]
	MOV DWORD PTR SS:[EBP-0x38],EAX
	MOV EDX,DWORD PTR SS:[EBP-0x38]
	MOVSS XMM0,DWORD PTR DS:[EDX+0x14]
	MOVSS DWORD PTR SS:[EBP-0x1C],XMM0
	MOVSS XMM0,DWORD PTR SS:[EBP-0x1C]
	MOVSS DWORD PTR SS:[EBP-0x30],XMM0
	MOV EAX,DWORD PTR SS:[EBP-0x38]
	MOVSS XMM0,DWORD PTR DS:[EAX+0x18]
	MOVSS DWORD PTR SS:[EBP-0x18],XMM0
	MOVSS XMM0,DWORD PTR SS:[EBP-0x18]
	MOVSS DWORD PTR SS:[EBP-0x2C],XMM0
	MOV DWORD PTR SS:[EBP-0x8],0x1
	JMP L039
	L036:
	MOV ECX,DWORD PTR SS:[EBP-0x8]
	ADD ECX,0x1
	MOV DWORD PTR SS:[EBP-0x8],ECX
	L039:
	MOV EDX,DWORD PTR SS:[EBP-0x8]
	CMP EDX,DWORD PTR SS:[EBP+0x0C]
	JGE L080
	MOV EAX,DWORD PTR SS:[EBP-0x8]
	MOV ECX,DWORD PTR SS:[EBP+0x8]
	MOVZX EDX,WORD PTR DS:[ECX+EAX*0x2]
	SHL EDX,0x7
	MOV EAX,DWORD PTR DS:[0x574A98]
	ADD EDX,DWORD PTR DS:[EAX+0x8]
	MOV DWORD PTR SS:[EBP-0x38],EDX
	MOV ECX,DWORD PTR SS:[EBP-0x38]
	MOVSS XMM0,DWORD PTR SS:[EBP-0x30]
	COMISS XMM0,DWORD PTR DS:[ECX+0x14]
	JBE L057
	MOV EDX,DWORD PTR SS:[EBP-0x38]
	MOVSS XMM0,DWORD PTR DS:[EDX+0x14]
	MOVSS DWORD PTR SS:[EBP-0x30],XMM0
	JMP L064
	L057:
	MOV EAX,DWORD PTR SS:[EBP-0x38]
	MOVSS XMM0,DWORD PTR DS:[EAX+0x14]
	COMISS XMM0,DWORD PTR SS:[EBP-0x1C]
	JBE L064
	MOV ECX,DWORD PTR SS:[EBP-0x38]
	MOVSS XMM0,DWORD PTR DS:[ECX+0x14]
	MOVSS DWORD PTR SS:[EBP-0x1C],XMM0
	L064:
	MOV EDX,DWORD PTR SS:[EBP-0x38]
	MOVSS XMM0,DWORD PTR SS:[EBP-0x2C]
	COMISS XMM0,DWORD PTR DS:[EDX+0x18]
	JBE L072
	MOV EAX,DWORD PTR SS:[EBP-0x38]
	MOVSS XMM0,DWORD PTR DS:[EAX+0x18]
	MOVSS DWORD PTR SS:[EBP-0x2C],XMM0
	JMP L079
	L072:
	MOV ECX,DWORD PTR SS:[EBP-0x38]
	MOVSS XMM0,DWORD PTR DS:[ECX+0x18]
	COMISS XMM0,DWORD PTR SS:[EBP-0x18]
	JBE L079
	MOV EDX,DWORD PTR SS:[EBP-0x38]
	MOVSS XMM0,DWORD PTR DS:[EDX+0x18]
	MOVSS DWORD PTR SS:[EBP-0x18],XMM0
	L079:
	JMP L036
	L080:
	MOVSS XMM0,DWORD PTR SS:[EBP-0x18]
	SUBSS XMM0,DWORD PTR SS:[EBP-0x2C]
	MOVSS XMM1,DWORD PTR SS:[EBP-0x1C]
	SUBSS XMM1,DWORD PTR SS:[EBP-0x30]
	COMISS XMM0,XMM1
	JBE L088
	MOV DWORD PTR SS:[EBP-0x3C],0x1
	JMP L089
	L088:
	MOV DWORD PTR SS:[EBP-0x3C],0x0
	L089:
	MOV EAX,DWORD PTR SS:[EBP-0x3C]
	MOV DWORD PTR SS:[EBP-0x4],EAX
	MOV ECX,DWORD PTR SS:[EBP-0x4]
	MOV EDX,DWORD PTR SS:[EBP-0x4]
	MOVSS XMM0,DWORD PTR SS:[EBP+ECX*0x4]
	SUBSS XMM0,DWORD PTR SS:[EBP+EDX*0x4]
	MOVSS XMM1,DWORD PTR DS:[__real1]
	COMISS XMM1,XMM0
	JB L110
	CALL Path_AllocPathnodeTree
	MOV DWORD PTR SS:[EBP-0x34],EAX
	MOV EAX,DWORD PTR SS:[EBP-0x34]
	MOV DWORD PTR DS:[EAX],-0x1
	MOV ECX,DWORD PTR SS:[EBP-0x34]
	MOV EDX,DWORD PTR SS:[EBP+0x0C]
	MOV DWORD PTR DS:[ECX+0x8],EDX
	MOV EAX,DWORD PTR SS:[EBP-0x34]
	MOV ECX,DWORD PTR SS:[EBP+0x8]
	MOV DWORD PTR DS:[EAX+0x0C],ECX
	MOV EAX,DWORD PTR SS:[EBP-0x34]
	JMP L298
	L110:
	MOV EDX,DWORD PTR SS:[EBP-0x4]
	MOV EAX,DWORD PTR SS:[EBP-0x4]
	MOVSS XMM0,DWORD PTR SS:[EBP+EDX*0x4]
	ADDSS XMM0,DWORD PTR SS:[EBP+EAX*0x4]
	MULSS XMM0,DWORD PTR DS:[__real2]
	MOVSS DWORD PTR SS:[EBP-0x28],XMM0
	MOV DWORD PTR SS:[EBP-0x20],0x0
	MOV ECX,DWORD PTR SS:[EBP+0x0C]
	SUB ECX,0x1
	MOV DWORD PTR SS:[EBP-0x24],ECX
	L120:
	MOV EDX,DWORD PTR SS:[EBP-0x20]
	CMP EDX,DWORD PTR SS:[EBP+0x0C]
	JL L134

	/*
	PUSH 0x0D39858
	PUSH 0x0C5C544
	PUSH 0x0
	PUSH 0x383
	PUSH 0x0D39284
	CALL 0xAssert_MyHandler
	ADD ESP,0x14
	MOVZX EAX,AL
	TEST EAX,EAX
	JNZ L134
	*/
	INT 3

	L134:
	CMP DWORD PTR SS:[EBP-0x24],0x0
	JGE L147

	/*
	PUSH 0x0D3984C
	PUSH 0x0C5C544
	PUSH 0x0
	PUSH 0x384
	PUSH 0x0D39284
	CALL 0xAssert_MyHandler
	ADD ESP,0x14
	MOVZX EDX,AL
	TEST EDX,EDX
	JNZ L147
	*/
	INT 3

	L147:
	MOV ECX,DWORD PTR SS:[EBP-0x20]
	MOV EDX,DWORD PTR SS:[EBP+0x8]
	MOVZX EAX,WORD PTR DS:[EDX+ECX*0x2]
	SHL EAX,0x7
	MOV ECX,DWORD PTR DS:[0x574A98]
	ADD EAX,DWORD PTR DS:[ECX+0x8]
	MOV EDX,DWORD PTR SS:[EBP-0x4]
	MOVSS XMM0,DWORD PTR SS:[EBP-0x28]
	COMISS XMM0,DWORD PTR DS:[EAX+EDX*0x4]
	JBE L175
	MOV EAX,DWORD PTR SS:[EBP-0x20]
	ADD EAX,0x1
	MOV DWORD PTR SS:[EBP-0x20],EAX
	MOV ECX,DWORD PTR SS:[EBP-0x20]
	CMP ECX,DWORD PTR SS:[EBP+0x0C]
	JL L174

	/*
	PUSH 0x0D39858
	PUSH 0x0C5C544
	PUSH 0x0
	PUSH 0x389
	PUSH 0x0D39284
	CALL 0xAssert_MyHandler
	ADD ESP,0x14
	MOVZX EDX,AL
	TEST EDX,EDX
	JNZ L174
	*/
	INT 3

	L174:
	JMP L147
	L175:
	MOV ECX,DWORD PTR SS:[EBP-0x24]
	MOV EDX,DWORD PTR SS:[EBP+0x8]
	MOVZX EAX,WORD PTR DS:[EDX+ECX*0x2]
	SHL EAX,0x7
	MOV ECX,DWORD PTR DS:[0x574A98]
	ADD EAX,DWORD PTR DS:[ECX+0x8]
	MOV EDX,DWORD PTR SS:[EBP-0x4]
	MOVSS XMM0,DWORD PTR DS:[EAX+EDX*0x4]
	COMISS XMM0,DWORD PTR SS:[EBP-0x28]
	JBE L201
	MOV EAX,DWORD PTR SS:[EBP-0x24]
	SUB EAX,0x1
	MOV DWORD PTR SS:[EBP-0x24],EAX
	JNS L200

	/*
	PUSH 0x0D3984C
	PUSH 0x0C5C544
	PUSH 0x0
	PUSH 0x38E
	PUSH 0x0D39284
	CALL 0xAssert_MyHandler
	ADD ESP,0x14
	MOVZX ECX,AL
	TEST ECX,ECX
	JNZ L200
	*/
	INT 3

	L200:
	JMP L175
	L201:
	MOV EAX,DWORD PTR SS:[EBP-0x20]
	CMP EAX,DWORD PTR SS:[EBP-0x24]
	JL L205
	JMP L226
	L205:
	MOV ECX,DWORD PTR SS:[EBP-0x20]
	MOV EDX,DWORD PTR SS:[EBP+0x8]
	MOV AX,WORD PTR DS:[EDX+ECX*0x2]
	MOV WORD PTR SS:[EBP-0x14],AX
	MOV ECX,DWORD PTR SS:[EBP-0x20]
	MOV EDX,DWORD PTR SS:[EBP+0x8]
	MOV EAX,DWORD PTR SS:[EBP-0x24]
	MOV ESI,DWORD PTR SS:[EBP+0x8]
	MOV AX,WORD PTR DS:[ESI+EAX*0x2]
	MOV WORD PTR DS:[EDX+ECX*0x2],AX
	MOV ECX,DWORD PTR SS:[EBP-0x24]
	MOV EDX,DWORD PTR SS:[EBP+0x8]
	MOV AX,WORD PTR SS:[EBP-0x14]
	MOV WORD PTR DS:[EDX+ECX*0x2],AX
	MOV ECX,DWORD PTR SS:[EBP-0x20]
	ADD ECX,0x1
	MOV DWORD PTR SS:[EBP-0x20],ECX
	MOV EDX,DWORD PTR SS:[EBP-0x24]
	SUB EDX,0x1
	MOV DWORD PTR SS:[EBP-0x24],EDX
	JMP L120
	L226:
	MOV EAX,DWORD PTR SS:[EBP-0x20]
	ADD EAX,DWORD PTR SS:[EBP-0x20]
	CMP EAX,DWORD PTR SS:[EBP+0x0C]
	JGE L246
	MOV ECX,DWORD PTR SS:[EBP-0x20]
	MOV EDX,DWORD PTR SS:[EBP+0x8]
	MOVZX EAX,WORD PTR DS:[EDX+ECX*0x2]
	SHL EAX,0x7
	MOV ECX,DWORD PTR DS:[0x574A98]
	ADD EAX,DWORD PTR DS:[ECX+0x8]
	MOV EDX,DWORD PTR SS:[EBP-0x4]
	MOVSS XMM0,DWORD PTR DS:[EAX+EDX*0x4]
	UCOMISS XMM0,DWORD PTR SS:[EBP-0x28]
	LAHF 
	TEST AH,0x44
	JPE L246
	MOV EAX,DWORD PTR SS:[EBP-0x20]
	ADD EAX,0x1
	MOV DWORD PTR SS:[EBP-0x20],EAX
	JMP L226
	L246:
	MOV ECX,DWORD PTR SS:[EBP-0x20]
	ADD ECX,DWORD PTR SS:[EBP-0x20]
	CMP ECX,DWORD PTR SS:[EBP+0x0C]
	JLE L266
	MOV EDX,DWORD PTR SS:[EBP-0x20]
	MOV EAX,DWORD PTR SS:[EBP+0x8]
	MOVZX ECX,WORD PTR DS:[EAX+EDX*0x2]
	SHL ECX,0x7
	MOV EDX,DWORD PTR DS:[0x574A98]
	ADD ECX,DWORD PTR DS:[EDX+0x8]
	MOV EAX,DWORD PTR SS:[EBP-0x4]
	MOVSS XMM0,DWORD PTR DS:[ECX+EAX*0x4]
	UCOMISS XMM0,DWORD PTR SS:[EBP-0x28]
	LAHF 
	TEST AH,0x44
	JPE L266
	MOV ECX,DWORD PTR SS:[EBP-0x20]
	SUB ECX,0x1
	MOV DWORD PTR SS:[EBP-0x20],ECX
	JMP L246
	L266:
	MOV EDX,DWORD PTR SS:[EBP-0x20]
	PUSH EDX
	MOV EAX,DWORD PTR SS:[EBP+0x8]
	PUSH EAX
	CALL L000
	ADD ESP,0x8
	MOV DWORD PTR SS:[EBP-0x10],EAX
	MOV ECX,DWORD PTR SS:[EBP+0x0C]
	SUB ECX,DWORD PTR SS:[EBP-0x20]
	PUSH ECX
	MOV EDX,DWORD PTR SS:[EBP-0x20]
	MOV EAX,DWORD PTR SS:[EBP+0x8]
	LEA ECX,DWORD PTR DS:[EAX+EDX*0x2]
	PUSH ECX
	CALL L000
	ADD ESP,0x8
	MOV DWORD PTR SS:[EBP-0xC],EAX
	CALL Path_AllocPathnodeTree
	MOV DWORD PTR SS:[EBP-0x34],EAX
	MOV EDX,DWORD PTR SS:[EBP-0x34]
	MOV EAX,DWORD PTR SS:[EBP-0x4]
	MOV DWORD PTR DS:[EDX],EAX
	MOV ECX,DWORD PTR SS:[EBP-0x34]
	MOVSS XMM0,DWORD PTR SS:[EBP-0x28]
	MOVSS DWORD PTR DS:[ECX+0x4],XMM0
	MOV EDX,DWORD PTR SS:[EBP-0x34]
	MOV EAX,DWORD PTR SS:[EBP-0x10]
	MOV DWORD PTR DS:[EDX+0x8],EAX
	MOV ECX,DWORD PTR SS:[EBP-0x34]
	MOV EDX,DWORD PTR SS:[EBP-0xC]
	MOV DWORD PTR DS:[ECX+0x0C],EDX
	MOV EAX,DWORD PTR SS:[EBP-0x34]
	L298:
	POP ESI
	MOV ESP,EBP
	POP EBP
	RETN 
	}
}

int __cdecl Path_CheckExpandedNodeVis(int (*iNodeNums)[8192], int *iNodeCount, char *visbuf)
{
	int iEntry; // [sp+8h] [bp-8h]@8

	for (int i = 0; i < *iNodeCount; ++i)
	{
		for (int j = 0; j < iNodeCount[1]; ++j)
		{
			if ((*iNodeNums)[i] == (*iNodeNums)[j + 0x2000])
				return 1;

			if ((*iNodeNums)[i] >= (*iNodeNums)[j + 0x2000])
				iEntry = NodeVisCacheEntry_0((*iNodeNums)[j + 0x2000], (*iNodeNums)[i]);
			else
				iEntry = NodeVisCacheEntry_0((*iNodeNums)[i], (*iNodeNums)[j + 0x2000]);

			if ((1 << (iEntry & 7)) & (unsigned __int8)visbuf[iEntry >> 3])
				return 1;
		}
	}

	return 0;
}

int __cdecl Path_GetExpandedNodeNums_r(int iNodeStart, float fDistLeft, int *iNodeNums, int iNodeCount)
{
	pathnode_t *node = Path_ConvertIndexToNode(iNodeStart);

	for (int i = 0; i < node->constant.totalLinkCount; ++i)
	{
		if (node->constant.Links[i].fDist <= fDistLeft)
		{
			int iNewNodeNum = node->constant.Links[i].nodeNum;

			for (int j = 0; j < iNodeCount; ++j)
			{
				if (iNodeNums[j] == iNewNodeNum)
					goto LABEL_2;
			}
			
			iNodeNums[iNodeCount] = iNewNodeNum;
			iNodeCount = Path_GetExpandedNodeNums_r(
				iNewNodeNum,
				fDistLeft - node->constant.Links[i].fDist,
				iNodeNums,
				iNodeCount + 1);
		}
	LABEL_2:
		;
	}

	return iNodeCount;
}

pathnode_t *__cdecl Path_ConvertIndexToNode(int index)
{
	//ASSERT(index < g_path.actualNodeCount);

	return &GameWorldCurrent->path.nodes[index];
}

int __cdecl Path_IsCoverNode(pathnode_t *node)
{
	ASSERT(node != nullptr);

	return (1 << node->constant.type) & 0x83FFC;
}

int __cdecl Path_IsValidClaimNode(pathnode_t *node)
{
	ASSERT(node != nullptr);

	return node->constant.spawnflags & 0x8000;
}

pathnode_tree_t *__cdecl Path_AllocPathnodeTree()
{
	++GameWorldCurrent->path.nodeTreeCount;
	return (pathnode_tree_t *)Hunk_AllocAlign(16, 4);// "Path_AllocPathnodeTree"
}

const int NodeVisCacheEntry_0(int i, int j)
{
	ASSERT(i < j);

	return j + GameWorldCurrent->path.nodeCount * i;
}

const int ExpandedNodeVisCacheEntry_0(int i, int j)
{
	ASSERT(i > j);

	return j + GameWorldCurrent->path.nodeCount * (i - 1);
}

bool Path_IsNegotiationLink(pathnode_t *pNodeFrom, pathnode_t *pNodeTo)
{
	return pNodeFrom->constant.type == NODE_NEGOTIATION_BEGIN
		&& pNodeTo->constant.type == NODE_NEGOTIATION_END
		&& pNodeFrom->constant.target == pNodeTo->constant.targetname;
}