#pragma once
#include <Windows.h>

struct XFile
{
	unsigned int size;
	unsigned int externalSize;
	unsigned int blockSize[7];
};

struct XAssetRawfileHeader
{
	int uncompressedSize;
	int compressedSize;
	BYTE fileData;
};

struct ScriptStringList
{
	int count;
	const char **strings;
};

struct XAssetList
{
	ScriptStringList stringList;
	int assetCount;
	void *assets;
};

struct XAsset
{
	int type;
	int header;
};

int FF_FFExtract(const char* filepath, const char* filename);