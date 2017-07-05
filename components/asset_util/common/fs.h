#pragma once
#include <fstream>
#include <functional>

#define FS_SEARCHPATTERN_IWD "*.IWD"
#define FS_SEARCHPATTERN_FF "*.FF"
//#define FS_SEARCHPATTERN_IWD "*.IWD"

const char* FS_Cwd(void);

bool FS_FileExists(const char* qpath);
//size_t FS_FileSize(const char* qpath);
long int FS_FileSize(const char* qpath);

const char* FS_GetExtensionSubString(const char* filename);
void FS_StripExtension(const char* in, char* out);
const char* FS_GetFilenameSubString(const char* pathname);
const wchar_t* FS_GetFilenameSubStringW(wchar_t* pathname);
void FS_StripFilename(const char* in, char* out);

typedef std::function<int(const char* filePath, const char* fileName)> FS_FileHandlerCallback_t;
typedef std::function<int(const char* path)> FS_DirectoryHandlerCallback_t;

int FS_FileCount(const char* path, const char* pattern);
int FS_FileIterator(const char* path, const char* pattern, FS_FileHandlerCallback_t FS_FileHandlerCallback);
int FS_DirectoryIterator(const char* path, FS_DirectoryHandlerCallback_t FS_DirectoryHandlerCallback);

int FS_CreatePath(const char* targetPath);
int FS_CopyDirectory(char* srcPath, char* destPath, bool overwriteFiles = false);

void FS_SanitizePath(char* path);

