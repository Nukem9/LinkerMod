#pragma once

#define FS_SEARCHPATTERN_IWD "*.IWD"

int FS_FileCount(const char* path, const char* pattern);
int FS_FileIterator(const char* path, const char* pattern, int(__cdecl* FS_FileHandlerCallback)(const char* filePath, const char* fileName));
