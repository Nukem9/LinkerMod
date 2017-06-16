#pragma once

enum ConChannel
{
	DEFAULT = 0,
	VERBOSE = 2,
};

typedef void (__cdecl * Com_Printf_t)(int level, const char *fmt, ...);

// level must be 2 for it to only print when -verbose is used
static Com_Printf_t Com_Printf = (Com_Printf_t)0x00407300;

typedef void (__cdecl * Com_Error_t)(int code, const char *fmt, ...);
static Com_Error_t Com_Error = (Com_Error_t)0x416810;
static Com_Printf_t Com_PrintError = (Com_Printf_t)0x416B30;

typedef int (__cdecl * FS_FOpenFile_t)(const char *qpath, int *fileHandle);
static FS_FOpenFile_t FS_FOpenFile = (FS_FOpenFile_t)0x4C9280;

typedef int (__cdecl * FS_FOpenFileReadCurrentThread_t)(const char *filename, int *file);
static FS_FOpenFileReadCurrentThread_t FS_FOpenFileReadCurrentThread = (FS_FOpenFileReadCurrentThread_t)0x4C9280;

typedef int (__cdecl * FS_FCloseFile_t)(int fileHandle);
static FS_FCloseFile_t FS_FCloseFile = (FS_FCloseFile_t)0x4C6720;

typedef int (__cdecl * FS_Read_t)(void *buffer, int len, int fileHandle);
static FS_Read_t FS_Read = (FS_Read_t)0x4C7480;

#include "com_files.h"
#include "com_memory.h"
#include "cm_trace.h"
#include "com_bsp_lump.h"
#include "com_bsp_load_obj.h"
#include "pathnode_load_obj.h"

struct GameWorldMp
{
	const char *name;
	PathData path;
};

static GameWorldMp **gameWorldCurrent = (GameWorldMp **)0x574A98;
#define GameWorldCurrent (*gameWorldCurrent)

static pathstatic_t *pathstatic = (pathstatic_t *)0x11BAC94;