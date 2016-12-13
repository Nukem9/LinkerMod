#pragma once

struct XFile
{
	unsigned int size;
	unsigned int externalSize;
	unsigned int blockSize[7];
};

typedef int (__cdecl* FS_FOpenFileRead_t)(const char *filename, int *file);
static FS_FOpenFileRead_t FS_FOpenFileRead = (FS_FOpenFileRead_t)0x004C6E20;

typedef void (__cdecl* FS_FCloseFile_t)(int h);
static FS_FCloseFile_t FS_FCloseFile = (FS_FCloseFile_t)0x0046CAA0;

typedef int (__cdecl* FS_Read_t)(void *buffer, int len, int h);
static FS_Read_t FS_Read = (FS_Read_t)0x004CFB60;
