#include "stdafx.h"

void Sys_WaitWorkerCmdInternal(jqWorkerCmd *name)
{
	((void (__cdecl *)(jqWorkerCmd *))0x00652140)(name);
}

void *jqLockData(jqBatch *batch)
{
	return ((void *(__cdecl *)(jqBatch *))0x004AB010)(batch);
}

void jqUnlockData(jqBatch *batch, void *data)
{
	((void(__cdecl *)(jqBatch *))0x005D7120)(batch);
}

bool jqPoll(jqBatchGroup *GroupID)
{
	return ((bool(__cdecl *)(jqBatchGroup *))0x008F1930)(GroupID);
}