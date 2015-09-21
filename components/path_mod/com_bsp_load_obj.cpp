#include "stdafx.h"

size_t* g_bspLen = (size_t*)0x0200BC6C;
BYTE** g_bspData = (BYTE**)0x0200BC68;
char g_bspName[65] = "";

size_t __cdecl Com_LoadBsp_EnforceVersion(const char* filename)
{
	if (!g_bspName[0])
		strcpy_s(g_bspName, filename);

	D3DBSP iBSP;
	iBSP.Load(*g_bspData);
	iBSP.Convert(BSPVERSION_COD_WAW);
	iBSP.Write(*g_bspData);
	*g_bspLen = iBSP.PotentialFileSize();

	return *g_bspLen;
}

void* rtn_Com_LoadBsp = (void*)0x0052106E;
void __declspec(naked) mfh_Com_LoadBsp()
{
	_asm
	{
		push[ebp + 8]
		call Com_LoadBsp_EnforceVersion
		add esp, 4
		push 0
		push eax
		jmp rtn_Com_LoadBsp
	}
}

void __cdecl SV_SavePaths(char* buf, int size)
{
	char path[MAX_PATH];
	sprintf_s(path, "raw/%s", g_bspName);

	D3DBSP iBSP;
	if (iBSP.Load(path) != 0)
		exit(-1);

	Lump newPaths;
	newPaths.AllocateMemory(size);
	memcpy(newPaths.content, buf, size);
	newPaths.size = size;
	newPaths.isEmpty = false;

	if (!iBSP.lumps[LUMP_PATHCONNECTIONS].isEmpty)
		iBSP.lumps[LUMP_PATHCONNECTIONS] = newPaths;
	else
		iBSP.AddLump(LUMP_PATHCONNECTIONS, newPaths);

	iBSP.Write(path);

	Com_UnloadBsp();
	Com_LoadBsp(g_bspName);
}