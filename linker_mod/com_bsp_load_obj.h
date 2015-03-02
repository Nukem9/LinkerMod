#pragma once

struct BspChunk
{
	LumpType type;
	unsigned int length;
};

struct BspHeader
{
	unsigned int ident;
	unsigned int version;
	unsigned int chunkCount;
	BspChunk chunks[100];
};

struct comBspGlob_t
{
	char name[64];
	BspHeader *header;
	unsigned int fileSize;
	unsigned int checksum;
	LumpType loadedLumpType;
	const void *loadedLumpData;
};

static comBspGlob_t *comBspGlob = (comBspGlob_t *)0x0057DC88;

bool Com_IsBspLoaded();
void Com_GetBspFilename(char *filename, int size, const char *mapname);
void Com_LoadBsp(const char *Filename);
const void *Com_LoadBspLump(const char *mapname, LumpType type, unsigned int elemSize, unsigned int *count);
void *Com_GetBspLump(LumpType type, unsigned int elemSize, unsigned int *count);
void Com_UnloadBspLump(LumpType type);