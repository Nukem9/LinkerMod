#pragma once
#include <fstream>

bool FS_FileExists(const char* qpath);
size_t FS_FileSize(const char* qpath);
const char* FS_GetExtensionSubString(const char* filename);
void FS_StripExtension(const char* in, char* out);
const char* FS_GetFilenameSubString(const char* pathname);
