#pragma once

struct XFile
{
	unsigned int size;
	unsigned int externalSize;
	unsigned int blockSize[7];
};

enum DESC_TYPE
{
	DESC_ERROR = 0,
	DESC_DESC = 1,
	DESC_JSON = 2,
};

#define MODDESC_LEN 254
#define DIRLIST_LEN 8192 * (MODDESC_LEN / 48)

extern char dirList[DIRLIST_LEN];

typedef int (__cdecl* FS_FOpenFileRead_t)(const char *filename, int *file);
static FS_FOpenFileRead_t FS_FOpenFileRead = (FS_FOpenFileRead_t)0x004C6E20;

typedef void (__cdecl* FS_FCloseFile_t)(int h);
static FS_FCloseFile_t FS_FCloseFile = (FS_FCloseFile_t)0x0046CAA0;

typedef int (__cdecl* FS_Read_t)(void *buffer, int len, int h);
static FS_Read_t FS_Read = (FS_Read_t)0x004CFB60;

typedef unsigned int (__cdecl* FS_FileRead_t)(void *ptr, unsigned int len, struct _iobuf *stream);
static FS_FileRead_t FS_FileRead = (FS_FileRead_t)0x0047F210;

typedef int (__cdecl* FS_SV_FOpenFileRead_t)(const char *filename, const char *dir, int *fp);
static FS_SV_FOpenFileRead_t FS_SV_FOpenFileRead = (FS_SV_FOpenFileRead_t)0x00464020;

typedef _iobuf *(__cdecl* FS_FileForHandle_t)(int f);
static FS_FileForHandle_t FS_FileForHandle = (FS_FileForHandle_t)0x00516DA0;

typedef char **(__cdecl* Sys_ListFiles_t)(const char *directory, const char *extension, const char *filter, int *numfiles, int wantsubs);
static Sys_ListFiles_t Sys_ListFiles = (Sys_ListFiles_t)0x00690E40;

typedef void (__cdecl* FS_FreeFileList_t)(const char **list, int allocTrackType);
static FS_FreeFileList_t FS_FreeFileList = (FS_FreeFileList_t)0x0067DFE0;

unsigned int __cdecl FS_ReadModDescription(void *ptr, unsigned int len, struct _iobuf *stream);

int __cdecl FS_GetModList(char *listbuf, int bufsize);
