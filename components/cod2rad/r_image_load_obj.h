#pragma once

struct GfxImageFileHeader_BO1
{
	char tag[3];
	char version;
	char format;
	char flags;
	__int16 dimensions[3];
	float gamma;
	int fileSizeForPicmip[8];
};
CHECK_SIZE(GfxImageFileHeader_BO1, 48);

struct GfxImageFileHeader_WAW
{
	char tag[3];
	char version;
	char format;
	char flags;
	__int16 dimensions[3];
	int fileSizeForPicmip[4];
};
CHECK_SIZE(GfxImageFileHeader_WAW, 28);

void mfh_Image_LoadFromFileWithReader();

/*
Detours::X86::DetourFunction((PBYTE)0x005675B0, (PBYTE)&hk_Image_LoadFromFileWithReader);
*/