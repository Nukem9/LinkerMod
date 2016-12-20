#pragma once 

#include <fstream>

static char* g_fmode = "wb+";

typedef FILE* (__cdecl* FS_FileOpen_t)(const char* filename, const char* mode);
extern FS_FileOpen_t FS_FileOpen;

typedef int(__cdecl* FS_FileClose_t)(FILE* file);
static FS_FileClose_t FS_FileClose = (FS_FileClose_t)0x00402883;


typedef int(__cdecl* FS_FTell_t)(FILE* file);
static FS_FTell_t FS_FTell = (FS_FTell_t)0x004042A4;

typedef int(__cdecl* FS_FSeek_t)(FILE* file, int offset, int origin);
static FS_FSeek_t FS_FSeek = (FS_FSeek_t)0x0040454E;


typedef size_t(__cdecl* FS_FileRead_t)(void *DstBuf, size_t ElementSize, size_t Count, FILE *File);
static FS_FileRead_t FS_FileRead = (FS_FileRead_t)0x00403F10;

typedef size_t(__cdecl* FS_FileWrite_t)(const void *DstBuf, size_t Size, size_t Count, FILE *File);
static FS_FileWrite_t FS_FileWrite = (FS_FileWrite_t)0x0040408A;

int __cdecl FS_ReadFile(const char *qpath, void **buffer);
void __cdecl FS_FreeFile(void *buffer);

size_t FS_FileGetFileSize(FILE* h);

void hk_FS_FOpenFileRead();

FILE* __cdecl FS_ImageRedirect(const char* filename, const char* mode);