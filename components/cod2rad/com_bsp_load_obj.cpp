#include "stdafx.h"

size_t __cdecl Com_LoadBsp_EnforceVersion(BYTE** data)
{
	if (!data)
		return -1;

	D3DBSP iBSP;
	iBSP.Load(*data);
	iBSP.Convert(BSPVERSION_COD_WAW);

	size_t len = iBSP.PotentialFileSize();
	Z_Free(*data);
	*data = (BYTE*)FS_AllocMem(len + 1);

	iBSP.Write(*data);

	return len;
}

void Com_SaveLightmaps_HDR(Lump* lump)
{
	BO1_LMAP* BO1Lmaps = (BO1_LMAP*)lump->content;

	for (DWORD i = 0; i < *g_LightmapCount; i++)
	{
		int p = 0;
		for (DWORD y = 0; y < 512; y++)
		{
			for (DWORD x = 0; x < 512; x++, p++)
			{
				vec4 p1 = LightmapBytes_HDR[0x40000 * i + 512 * y + x];
				vec4 p2 = Lightmap2Bytes_HDR[0x40000 * i + 512 * y + x];

				BO1Lmaps[i].small_lmap_1[p].R = p1.r;
				BO1Lmaps[i].small_lmap_1[p].G = p1.g;
				BO1Lmaps[i].small_lmap_1[p].B = p1.b;
				BO1Lmaps[i].small_lmap_1[p].A = p1.a;

				BO1Lmaps[i].small_lmap_2[p].R = p2.r;
				BO1Lmaps[i].small_lmap_2[p].G = p2.g;
				BO1Lmaps[i].small_lmap_2[p].B = p2.b;
				BO1Lmaps[i].small_lmap_2[p].A = p2.a;
			}
		}
	}

	delete[] LightmapBytes_HDR;
	delete[] Lightmap2Bytes_HDR;
}

int __cdecl Com_SaveBsp_EnforceVersion(FILE* h)
{
	long len = FS_FTell(h);
	BYTE* buf = new BYTE[len];

	FS_FSeek(h, 0, SEEK_SET);
	FS_FileRead(buf, 1, len, h);

	D3DBSP* iBSP = new D3DBSP;
	iBSP->Load(buf);
	delete[] buf;

	iBSP->Convert(BSPVERSION_COD_BO);

	if (g_HDR)
	{
		Lump* lightmaps = NULL;
		for (unsigned int i = 0; i < iBSP->diskLumpCount; i++)
		{
			if (iBSP->diskLumpOrder[i] == LUMP_LIGHTBYTES)
			{
				lightmaps = &iBSP->lumps[i];
				break;
			}
		}
		
		Com_SaveLightmaps_HDR(lightmaps);
	}

	len = iBSP->PotentialFileSize();
	buf = new BYTE[len];
	iBSP->Write(buf);
	delete iBSP;

	FS_FSeek(h, 0, SEEK_SET);
	FS_FileWrite(buf, 1, len, h);

	return FS_FileClose(h);
}

void* rtn_Com_SaveBsp = (void*)0x00442E9F;
void __declspec(naked) mfh_Com_SaveBsp()
{
	_asm
	{
		call Com_SaveBsp_EnforceVersion
		add esp, 4
		jmp rtn_Com_SaveBsp
	}
}