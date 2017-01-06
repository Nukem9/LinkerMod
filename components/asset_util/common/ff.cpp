#include "ff.h"
#include "../cvar.h"
#include "fs.h"
#include "io.h"
#include "str.h"
#include <Windows.h>
#include "../sys/AppInfo.h"
#include "zlib\zlib.h"

char* FindRawfileString(BYTE* start, BYTE* end)
{
	while (start < end - 5)
	{
		if (strncmp(".atr", (char*)start, 4) == 0 ||
			strncmp(".gsc", (char*)start, 4) == 0 ||
			strncmp(".csc", (char*)start, 4) == 0 ||
			strncmp(".wav", (char*)start, 4) == 0)
		{
			return (char*)start;
		}

		if (start < end - 8 && strncmp(".vision", (char*)start, 7) == 0)
		{
			return (char*)start;
		}

		start++;
	}

	return nullptr;
}

char* FindRawfileStringReverseLookup(BYTE* start)
{
	for (char* strStart = (char*)start; strStart > strStart - MAX_PATH; strStart--)
	{
		if (*(DWORD*)strStart == 0xFFFFFFFF)
		{
			if (*(strStart + 4))
			{
				return strStart + 4;
			}
			
			return nullptr;
		}
	}

	return nullptr;
}

int FF_FFExtractCompressedRawfile(XAssetRawfileHeader* rawfileHeader, const char* rawfilePath)
{
	Con_Print_v("Extracting file: \"%s\"...	", rawfilePath);

	char qpath[1024] = "";
	sprintf_s(qpath, "%s/%s", AppInfo_OutDir(), rawfilePath);

	//
	// If not in overwrite mode AND the file exists
	// skip it before performing decompression
	//
	if (!fs_overwrite.ValueBool())
	{
		if (FS_FileExists(qpath))
		{
			Con_Print_v("SKIPPED\n");
			return 0;
		}
	}

	if (FS_CreatePath(rawfilePath) != 0)
	{
		Con_Error_v("ERROR\n");
		return 0;
	}

	//
	// Catch incorrect rawfile data to prevent massive allocations
	//
	if (rawfileHeader->uncompressedSize > 1024 * 1024 * 16)
	{
		Con_Print_v("IGNORED\n");
		return 0;
	}

	BYTE* dBuf = new BYTE[rawfileHeader->uncompressedSize];
	unsigned long dSize = rawfileHeader->uncompressedSize;
	if (uncompress(dBuf, &dSize, &rawfileHeader->fileData, rawfileHeader->compressedSize) != 0)
	{
		Con_Error_v("READ ERROR\n");
		delete[] dBuf;
		return 0;
	}

	if (FILE* h = fopen(qpath, "wb"))
	{
		fwrite(dBuf, 1, rawfileHeader->uncompressedSize, h);
		fclose(h);

		Con_Print_v("SUCCESS\n");

		delete[] dBuf;
		return 1;
	}

	delete[] dBuf;

	Con_Error_v("WRITE ERROR\n");
	return 0;
}

int FF_FFExtractUncompressedRawfile(char* rawfileData, const char* rawfilePath)
{
	Con_Print_v("Extracting file: \"%s\"...	", rawfilePath);

	char qpath[1024] = "";
	sprintf_s(qpath, "%s/%s", AppInfo_OutDir(), rawfilePath);

	//
	// If not in overwrite mode AND the file exists
	// skip it before performing decompression
	//
	if (!fs_overwrite.ValueBool())
	{
		if (FILE* h = fopen(qpath, "r"))
		{
			Con_Print_v("SKIPPED\n");

			fclose(h);
			return 0;
		}
	}

	if (FS_CreatePath(rawfilePath) != 0)
	{
		Con_Error_v("PATH ERROR\n");
		return 0;
	}

	//
	// Catch incorrect rawfile data to prevent massive allocations
	//
	if (strlen(rawfileData) > 1024 * 1024 * 16)
	{
		Con_Print_v("IGNORED\n");
		return 0;
	}

	if (FILE* h = fopen(qpath, "wb"))
	{
		fwrite(rawfileData, 1, strlen(rawfileData), h);
		fclose(h);

		Con_Print_v("SUCCESS\n");
		return strlen(rawfileData);
	}

	Con_Error_v("ERROR\n");
	return 0;
}

int FF_FFExtractSoundFile(Snd_Header* snd_header, const char* sndfilePath)
{
	Con_Print_v("Extracting file: \"%s\"...	\n", sndfilePath);

	/*Con_Print_v("Format: %d\n", snd_header->format);
	Con_Print_v("Size: %d (0x%X)\n", snd_header->data_size, snd_header->data_size);
	Con_Print_v("Seek Table Size: %d\n", snd_header->seek_table_count);*/

	char qpath[1024] = "";
#if _DEBUG
	sprintf_s(qpath, "%s", sndfilePath);
#else
	sprintf_s(qpath, "%s/%s", AppInfo_OutDir(), sndfilePath);
#endif
	
	

	//
	// If not in overwrite mode AND the file exists
	// skip it before performing decompression
	//
	if (!fs_overwrite.ValueBool())
	{
		if (FILE* h = fopen(qpath, "r"))
		{
			Con_Print_v("SKIPPED\n");

			fclose(h);
			return 0;
		}
	}

	if (FS_CreatePath(sndfilePath) != 0)
	{
		Con_Error_v("PATH ERROR\n");
		return 0;
	}

	//
	// Catch incorrect sndfile data and bypass it
	//
	if (snd_header->format != 6 && snd_header->format != 7)
	{
		Con_Print_v("IGNORED\n");
		return 0;
	}
	
	//
	// Ignore non-Loaded Sounds Assets
	//
	if (snd_header->data_size == 0)
	{
		Con_Print_v("IGNORED\n");
		return 0;
	}

	if (FILE* h = fopen(qpath, "wb"))
	{
		fwrite(snd_header, 1, sizeof(Snd_Header), h);

		int* seek_table = (int*)(sndfilePath + strlen(sndfilePath) + 1);

		if (snd_header->seek_table_count != 0)
		{
			fwrite(seek_table, 4, snd_header->seek_table_count, h);
		}

		//
		// Pad the seek table to 2040 bytes
		//
		BYTE pad[2040];
		memset(pad, 0, 2040);
		fwrite(pad, 1, 2040 - 4 * snd_header->seek_table_count, h);
		
		BYTE* snd_data = (BYTE*)&seek_table[snd_header->seek_table_count];
		fwrite(snd_data, 1, snd_header->data_size, h);

		fclose(h);

		Con_Print_v("SUCCESS\n");
		return snd_header->data_size;
	}

	Con_Error_v("ERROR\n");
	return 0;
}

int FF_FFExtractFiles(BYTE* searchData, DWORD searchSize)
{
	int extractedFileCount = 0;

	BYTE* endofBuffer = searchData + searchSize;

	BYTE* lastSearchLoc = 0;
	while (searchData < searchData + searchSize)
	{
		char* rawfileString = FindRawfileString(searchData, endofBuffer);

		if (!rawfileString)
		{
			return extractedFileCount;
		}

		char* tmpString = FindRawfileStringReverseLookup((BYTE*)rawfileString);

		if (!tmpString)
		{
			return extractedFileCount;
		}

		if ((BYTE*)tmpString < searchData || !IsCharAlphaNumericA(*tmpString))
		{
			searchData += strlen(rawfileString) + 1;
			continue;
		}

		rawfileString = tmpString;

		if (Str_EndsWith(rawfileString, ".wav"))
		{
			if (!g_extractSounds.ValueBool())
			{
				searchData = (BYTE*)rawfileString + strlen(rawfileString) + 1;
				continue;
			}

			Snd_Header* snd_info = (Snd_Header*)(rawfileString - sizeof(Snd_Header));
			FF_FFExtractSoundFile(snd_info, rawfileString);
			searchData = (BYTE*)rawfileString + strlen(rawfileString) + 1;
		}

		//
		// ARG_FLAG_FF Should never been false if this function is running
		//
		/*
		if (!ARG_FLAG_FF)
		{
			searchData = (BYTE*)rawfileString + strlen(rawfileString) + 1;
			continue;
		}
		*/

		else if (Str_EndsWith(rawfileString, ".vision"))
		{
			char* rawfileData = rawfileString + strlen(rawfileString) + 1;
			int fileLen = FF_FFExtractUncompressedRawfile(rawfileData, rawfileString);

			if (!fileLen)
			{
				searchData = (BYTE*)rawfileString + strlen(rawfileString) + 1;
				continue;
			}
			
			searchData = (BYTE*)rawfileData + fileLen + 1;
		}
		else
		{
			XAssetRawfileHeader* rawfileHeader = (XAssetRawfileHeader*)(rawfileString + strlen(rawfileString) + 1);
			if (!FF_FFExtractCompressedRawfile(rawfileHeader, rawfileString))
			{
				searchData = (BYTE*)rawfileString + strlen(rawfileString) + 1;
				continue;
			}

			
			searchData = (BYTE*)rawfileHeader + rawfileHeader->compressedSize;
		}

		extractedFileCount++;
	}

	return extractedFileCount;
}

int FF_FFExtract(const char* filepath, const char* filename)
{
	Con_Print("Extracting rawfiles from \"%s\"...\n", filename);

	FILE* h = nullptr;
	if (fopen_s(&h, filepath, "r+b") != 0)
	{
		Con_Error("ERROR: Fastfile '%s' could not be found\n\n", filepath);
		return FALSE;
	}
	rewind(h);

	fseek(h, 0, SEEK_END);
	size_t fileSize = ftell(h);

	// Get Compressed FileSize and Allocate a Storage Buffer for Compressed Data
	size_t cSize = fileSize - 12;
	BYTE* cBuf = new BYTE[cSize | 0x8000];

	fseek(h, 12, SEEK_SET);
	fread(cBuf, 1, cSize, h);

	XFile ffInfo;
	unsigned long dSize = sizeof(XFile);
	uncompress((BYTE*)&ffInfo, &dSize, cBuf, 0x8000);

	dSize = ffInfo.size + 36;
	if (dSize >= 1073741824)
	{
		//Any fastfiles that claim they decompress to a file >= 1GB
		//are either corrupt or do not belong to the vanilla game
		Con_Error("ERROR: Skipping %s\n", filename);
		return 1;
	}

	BYTE* dBuf = new BYTE[dSize];
	uncompress(dBuf, &dSize, cBuf, cSize);
	delete[] cBuf;


	/*XAssetList* xal = (XAssetList*)(dBuf + 36);
	Con_Print("XAssetList\n");
	Con_Print("	StringList\n");
	Con_Print("		count 0x%X %d\n", xal->stringList.count, xal->stringList.count);
	Con_Print("		strings 0x%X\n", xal->stringList.strings);
	Con_Print("	assetCount 0x%X %d\n", xal->assetCount, xal->assetCount);
	Con_Print("	assets 0x%X\n", xal->assets);

	char* stringList = (char*)(dBuf + 36 + sizeof(XAssetList));
	stringList += xal->stringList.count * sizeof(char*);

	for (int i = 0; i < xal->stringList.count; i++)
	{
		//fix for fastfiles where the count doesnt match
		//the array size
		if (!stringList[1])
		{
			DWORD* tmp = (DWORD*)stringList;
			if (tmp[1] == 0xFFFFFFFF)
			{
				break;
			}
		}

		stringList += strlen(stringList) + 1;
	}
	
	XAsset* AassetList = (XAsset*)stringList;

	int assetTypeCounter[0x1D];
	memset(assetTypeCounter, 0, 128 * 4);

	XAsset* assetList = (XAsset*)stringList;
	for (int i = 0; i < xal->assetCount; i++)
	{
		assetTypeCounter[assetList->type]++;
		assetList++;
	}*/

	FF_FFExtractFiles((BYTE*)dBuf, ffInfo.size + 36);

	delete[] dBuf;
	return 0;
}

int __cdecl FF_DirectoryHandler(const char* path)
{
	return FS_FileIterator(path, FS_SEARCHPATTERN_FF, FF_FFExtract);
}
