#pragma once

/* 4032 */
struct BspChunk
{
	LumpType type;
	unsigned int length;
};

/* 4033 */
struct BspHeader
{
	unsigned int ident;
	unsigned int version;
	unsigned int chunkCount;
	BspChunk chunks[100];
};

/* 4034 */
struct comBspGlob_t
{
	char name[64];
	BspHeader *header;
	unsigned int fileSize;
	unsigned int checksum;
	LumpType loadedLumpType;
	const void *loadedLumpData;
} static *comBspGlob = (comBspGlob_t *)0x57DC88;

typedef void *(__cdecl * Com_GetBspLump_t)(LumpType type, unsigned int elemSize, unsigned int *count);
static Com_GetBspLump_t Com_GetBspLump = (Com_GetBspLump_t)0x4017D0;

bool Com_IsBspLoaded();
void Com_GetBspFilename(char *filename, int size, const char *mapname);
void Com_LoadBsp(const char *Filename);
const void *Com_LoadBspLump(const char *mapname, LumpType type, unsigned int elemSize, unsigned int *count);
void Com_UnloadBspLump(LumpType type);