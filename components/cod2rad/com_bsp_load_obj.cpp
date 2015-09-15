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