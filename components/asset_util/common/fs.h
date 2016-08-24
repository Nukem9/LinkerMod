#pragma once
#include <fstream>

#define FS_SEARCHPATTERN_IWD "*.IWD"
#define FS_SEARCHPATTERN_FF "*.FF"
//#define FS_SEARCHPATTERN_IWD "*.IWD"

const char* FS_Cwd(void);

bool FS_FileExists(const char* qpath);
size_t FS_FileSize(const char* qpath);

const char* FS_GetExtensionSubString(const char* filename);
void FS_StripExtension(const char* in, char* out);
const char* FS_GetFilenameSubString(const char* pathname);

int FS_FileCount(const char* path, const char* pattern);
int FS_FileIterator(const char* path, const char* pattern, int(__cdecl* FS_FileHandlerCallback)(const char* filePath, const char* fileName));
int FS_DirectoryIterator(const char* path, int(__cdecl* FS_DirectoryHandlerCallback)(const char* path));

int FS_CreatePath(const char* targetPath);
int FS_CopyDirectory(char* srcPath, char* destPath, bool overwriteFiles = false);


