#pragma once

#define FS_SEARCHPATTERN_IWD "*.IWD"
#define FS_SEARCHPATTERN_FF "*.FF"

const char* FS_Cwd(void);

int FS_FileCount(const char* path, const char* pattern);
int FS_FileIterator(const char* path, const char* pattern, int(__cdecl* FS_FileHandlerCallback)(const char* filePath, const char* fileName));
int FS_DirectoryIterator(const char* path, int(__cdecl* FS_DirectoryHandlerCallback)(const char* path));

//
// Tests a path to a file or directory
// return 0 - invalid file
// return 1 - file
// return 2 - directory
//
int FS_TestPath(const char* path);

int FS_CreatePath(const char* targetPath);
int FS_CopyDirectory(char* srcPath, char* destPath, bool overwriteFiles = false);
