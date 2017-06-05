/*
	This header file contains the structure offsets struct, system initializers / destructors, and other process info that is relevent to only the ripper command and not asset_util as a whole
*/

#pragma once
#include "../../sys/process.h"
#include "db_registry.h"
#include "foreign_ptr.h"

#include "dvar.h"

struct ProcessInfo
{
	HANDLE handle;
	processId_t pid;

	ForeignPointer<unsigned __int16> db_hashTable;
	ForeignPointer<XAssetEntryPoolEntry> db_assetEntryPool;
	ForeignPointer<XZoneName> db_zoneNames;

	ForeignPointer<dvar_s*> cl_ingame;
};

extern ProcessInfo g_process;

int ProcessInfo_Init(processId_t pid);
void ProcessInfo_Free(void);
