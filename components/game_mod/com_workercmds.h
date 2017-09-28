#pragma once

struct jqBatch
{
	char _pad1[0xC];
	int Module;
};

struct jqWorkerCmd
{
};

struct jqBatchGroup
{
};

struct WorkerCmdConditional
{
};

typedef void Sys_AddWorkerCmdInternal_t(jqWorkerCmd *name, void *data, WorkerCmdConditional *cond);
static Sys_AddWorkerCmdInternal_t* Sys_AddWorkerCmdInternal = (Sys_AddWorkerCmdInternal_t*)0x005317A0;

void Sys_WaitWorkerCmdInternal(jqWorkerCmd *name);

void *jqLockData(jqBatch *batch);
void jqUnlockData(jqBatch *batch, void *data);
bool jqPoll(jqBatchGroup *GroupID);