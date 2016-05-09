#include "stdafx.h"

void __cdecl Image_LoadFromFileWithReader_EnforceVersion6(GfxImageFileHeader_BO1* oldHeader)
{
	GfxImageFileHeader_WAW* newHeader = (GfxImageFileHeader_WAW*)oldHeader;

	if (oldHeader->version == 6)
	{
		oldHeader->version = 13;
		return;
	}
	else if (oldHeader->version != 13)
	{
		return;
	}

	for (int i = 0; i < 4; i++)
	{
		newHeader->fileSizeForPicmip[i] = oldHeader->fileSizeForPicmip[i] - sizeof(GfxImageFileHeader_BO1);
	}

	memcpy(&newHeader[1], &oldHeader[1], newHeader->fileSizeForPicmip[0]);

	for (int i = 0; i < 4; i++)
	{
		newHeader->fileSizeForPicmip[i] += sizeof(GfxImageFileHeader_WAW);
	}
}

void* rtn_Image_LoadFromFileWithReader_Error = (void*)0x00417AC3;

void* rtn_Image_LoadFromFileWithReader = (void*)0x00417AA5;
void __declspec(naked) mfh_Image_LoadFromFileWithReader() //0x00417A91
{
	__asm
	{
		pushad
		push edi
		call Image_LoadFromFileWithReader_EnforceVersion6
		pop edi
		popad

		cmp [edi+0], 'I'
		jnz IMAGE_ERROR
		cmp [edi+1], 'W'
		jnz IMAGE_ERROR
		cmp [edi+2], 'i'
		jnz IMAGE_ERROR
		mov al, [edi+3]

		jmp rtn_Image_LoadFromFileWithReader

	IMAGE_ERROR:
		jmp rtn_Image_LoadFromFileWithReader_Error
	}
}
