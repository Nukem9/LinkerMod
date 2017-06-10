#pragma once

#include <fstream>

enum FsListBehavior_e
{
	FS_LIST_PURE_ONLY = 0x0,
	FS_LIST_ALL = 0x1,
};

typedef int  __cdecl fclose_t(FILE *File);
static fclose_t* o_fclose = (fclose_t*)0x004F3C80;

void mfh_fcloseDeps();

const char ** FS_ListFiles(const char *path, const char *extension, FsListBehavior_e behavior, int *numfiles, int allocTrackType);
void FS_FreeFileList(const char **list, int allocTrackType=0);
