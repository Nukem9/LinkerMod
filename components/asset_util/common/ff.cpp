#include "ff.h"
#include "../cvar.h"
#include "fs.h"
#include "io.h"
#include "str.h"
#include <Windows.h>
#include "../sys/AppInfo.h"
#include "zlib\zlib.h"
#include <vector>
#include <algorithm>
#include <ppl.h>

enum class RAWFILE_TYPE
{
	COMPRESSED,
	UNCOMPRESSED,
	SOUND,

	NONE = -1,
};

RAWFILE_TYPE FindRawfileString(BYTE* start, BYTE* end, char** result)
{
	RAWFILE_TYPE type = RAWFILE_TYPE::NONE;

	while (start < end - 5)
	{
		if (strncmp(".gsc", (char*)start, 4) == 0 ||
			strncmp(".csc", (char*)start, 4) == 0)
		{
			type = RAWFILE_TYPE::COMPRESSED;
		}
		else if (strncmp(".atr", (char*)start, 4) == 0 ||
				strncmp(".sun", (char*)start, 4) == 0 ||
				strncmp(".xpo", (char*)start, 4) == 0 )
		{
			type = RAWFILE_TYPE::UNCOMPRESSED;
		}
		else if (strncmp(".wav", (char*)start, 4) == 0)
		{
			type = RAWFILE_TYPE::SOUND;
		}
		else if (start < end - 8 && strncmp(".vision", (char*)start, 7) == 0)
		{
			type = RAWFILE_TYPE::UNCOMPRESSED;
		}

		if (type != RAWFILE_TYPE::NONE)
		{
			*result = (char*)start;
			return type;
		}

		start++;
	}

	*result = nullptr;
	return RAWFILE_TYPE::NONE;
}

char* FindRawfileStringReverseLookup(BYTE* start)
{
	for (char* strStart = (char*)start; strStart > strStart - MAX_PATH; strStart--)
	{
		if (*strStart == '\0')
			return strStart + 1;
		else if (*(DWORD*)strStart == 0xFFFFFFFF)
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

	int* pHeader = (int*)rawfilePath;
	int len = pHeader[-2];

	//
	// Catch incorrect rawfile data to prevent massive allocations
	//
	if (len > 1024 * 1024 * 16)
	{
		Con_Print_v("IGNORED\n");
		return 0;
	}

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
		printf("%s\n", rawfilePath);
		Con_Error_v("PATH ERROR\n");
		return 0;
	}
	
	if (FILE* h = fopen(qpath, "wb"))
	{
		fwrite(rawfileData, 1, len, h);
		fclose(h);

		Con_Print_v("SUCCESS\n");
		return len;
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
		Con_Print_v("IGNORED (fmt %d)\n", snd_header->format);
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

void PerformLookup(BYTE *Buffer, std::vector<BYTE>& Data, std::vector<BYTE>& Signature)
{
	for (auto scanStart = Data.begin();;)
	{
		// Search for the pattern
		auto ret = std::search(scanStart, Data.end(), Signature.begin(), Signature.end());

		// If we didn't find a match, exit
		if (ret == Data.end())
			break;

		// Convert match index to a real address
		BYTE *asset = std::distance(Data.begin(), ret) + Buffer;

		// Parse the asset (reverse string scan for the name)
		char *rawfileString = (char *)asset;
		char* tmpString = FindRawfileStringReverseLookup((BYTE*)rawfileString);
		int moveLen = 0;

		if (!tmpString)
			return;

		if ((BYTE*)tmpString < Buffer || !IsCharAlphaNumericA(*tmpString))
		{
			scanStart = ret + strlen(rawfileString) + 1;
			continue;
		}

		rawfileString = tmpString;

		char assetExt[32];
		memset(assetExt, 0, sizeof(assetExt));
		memcpy(assetExt, asset, Signature.size());

		// Dump
		if (strstr(assetExt, ".gsc") ||
			strstr(assetExt, ".csc"))
		{
			// GameScripts are compressed
			XAssetRawfileHeader* rawfileHeader = (XAssetRawfileHeader*)(rawfileString + strlen(rawfileString) + 1);
			if (FF_FFExtractCompressedRawfile(rawfileHeader, rawfileString))
				moveLen = rawfileHeader->compressedSize;
		}
		else if (strstr(assetExt, ".atr") ||
			strstr(assetExt, ".sun") ||
			strstr(assetExt, ".xpo") ||
			strstr(assetExt, ".txt") ||
			strstr(assetExt, ".cfg") ||
			strstr(assetExt, ".vision"))
		{
			// Ignore menu .txt files stored in /ui/ or /ui_mp/
			if (!(strstr(assetExt, ".txt") && (!_strnicmp(rawfileString, "ui/", 3) || !_strnicmp(rawfileString, "ui_mp/", 6))))
			{
				// Random uncompressed types
				char* rawfileData = rawfileString + strlen(rawfileString) + 1;
				moveLen = FF_FFExtractUncompressedRawfile(rawfileData, rawfileString);
			}
		}
		else if (strstr(assetExt, ".wav"))
		{
			// Audio files
			Snd_Header* snd_info = (Snd_Header*)(rawfileString - sizeof(Snd_Header));
			moveLen = FF_FFExtractSoundFile(snd_info, rawfileString);
		}
		else if (strstr(assetExt, ".hlsl"))
		{
			// Shaders (ignored for now)
		}

		scanStart = (ret + moveLen + 1);
	}
}

int FF_FFExtractFiles(BYTE* searchData, DWORD searchSize)
{
	auto data = std::vector<BYTE>(searchData, searchData + searchSize);
	auto scanList = std::vector<std::vector<BYTE>>();

	scanList.push_back(std::vector<BYTE>({ '.', 'g', 's', 'c' }));
	scanList.push_back(std::vector<BYTE>({ '.', 'c', 's', 'c' }));
	scanList.push_back(std::vector<BYTE>({ '.', 'a', 't', 'r' }));
	scanList.push_back(std::vector<BYTE>({ '.', 's', 'u', 'n' }));
	scanList.push_back(std::vector<BYTE>({ '.', 'x', 'p', 'o' }));
	scanList.push_back(std::vector<BYTE>({ '.', 'w', 'a', 'v' }));
	scanList.push_back(std::vector<BYTE>({ '.', 'c', 'f', 'g' }));
	scanList.push_back(std::vector<BYTE>({ '.', 't', 'x', 't' }));
	scanList.push_back(std::vector<BYTE>({ '.', 'v', 'i', 's', 'i', 'o', 'n' }));
	// scanList.push_back(std::vector<BYTE>({ '.', 'h', 'l', 's', 'l' }));

	if (g_extractSounds.ValueBool())
		scanList.push_back(std::vector<BYTE>({ '.', 'w', 'a', 'v' }));

	concurrency::parallel_for(size_t(0), scanList.size(), [&](size_t i)
	{
		PerformLookup(searchData, data, scanList[i]);
	});

	// Return value is never used as of this time
	return 0;
}

int FF_FFExtract(const char* filepath, const char* filename)
{
	Con_Print("Extracting rawfiles from \"%s\"...\n", filename);

	FILE* h = nullptr;
	if (fopen_s(&h, filepath, "r+b") != 0)
	{
		Con_Error("ERROR: Fastfile '%s' could not be found\n", filepath);
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
