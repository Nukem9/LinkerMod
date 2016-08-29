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

struct Snd_Header
{
	unsigned int version;
	unsigned int frame_count;
	unsigned int frame_rate;
	unsigned int channel_count;
	unsigned int header_size;
	unsigned int block_size;
	unsigned int buffer_size;
	int format;
	int channel_flags;
	int flags;
	unsigned int seek_table_count;
	unsigned int *seek_table;
	unsigned int data_size;
	char *data;
};

int FF_FFExtract(const char* filepath, const char* filename);
int __cdecl FF_DirectoryHandler(const char* path);
