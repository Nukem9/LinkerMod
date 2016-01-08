#pragma once

#define FS_SEARCHPATTERN_IWD "*.IWD"
#define FS_SEARCHPATTERN_FF "*.FF"
//#define FS_SEARCHPATTERN_IWD "*.IWD"

int FS_FileCount(const char* path, const char* pattern);
int FS_FileIterator(const char* path, const char* pattern, int(__cdecl* FS_FileHandlerCallback)(const char* filePath, const char* fileName));
int FS_CreatePath(const char* targetPath);