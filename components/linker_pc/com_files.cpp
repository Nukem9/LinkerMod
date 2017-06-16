#include "stdafx.h"

struct searchpath_s;

VANILLA_VALUE(fs_searchpaths, searchpath_s*, 0x011CB4C4);

typedef const char **(__cdecl* FS_ListFilteredFiles_t)(searchpath_s *searchPath, const char *path, const char *extension, const char *filter, FsListBehavior_e behavior, int *numfiles, int allocTrackType);
static FS_ListFilteredFiles_t FS_ListFilteredFiles = (FS_ListFilteredFiles_t)0x004C7D50;

FILE** fDeps = (FILE**)0x010133C8;
void* sub_41DDD0 = (void*)0x0041DDD0;

void* rtn_fcloseDeps = (void*)0x0041E68F;
void __declspec(naked) mfh_fcloseDeps()
{
	_asm
	{
		push edx
		mov edx, [fDeps]
		push edx
		call o_fclose
		add esp, 4
		pop edx
		call sub_41DDD0
		push ebx
		jmp rtn_fcloseDeps
	}
}

const char ** FS_ListFiles(const char *path, const char *extension, FsListBehavior_e behavior, int *numfiles, int allocTrackType)
{
	return FS_ListFilteredFiles(fs_searchpaths, path, extension, 0, behavior, numfiles, allocTrackType);
}

void FS_FreeFileList(const char **list, int allocTrackType)
{
	if (list)
		Hunk_UserDestroy((HunkUser *)*(list - 1));
}