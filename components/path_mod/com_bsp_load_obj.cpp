#include "stdafx.h"

size_t* g_bspLen = (size_t*)0x0200BC6C;
BYTE** g_bspData = (BYTE**)0x0200BC68;
char g_bspName[MAX_PATH] = "";

void __cdecl Com_LoadBsp_EnforceVersion(const char* filename)
{
	if (!g_bspName[0])
		strcpy_s(g_bspName, filename);

	D3DBSP iBSP;
	iBSP.Load(*g_bspData);
	iBSP.Convert(BSP_VERSION_COD_WAW);
	iBSP.Write(*g_bspData);
	*g_bspLen = iBSP.PotentialFileSize();
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
		push [g_bspLen]
		jmp rtn_Com_LoadBsp
	}
}