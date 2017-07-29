#pragma once

struct jqWorkerCmd
{
};

struct WorkerCmdConditional
{
};

typedef void Sys_AddWorkerCmdInternal_t(jqWorkerCmd *name, void *data, WorkerCmdConditional *cond);
static Sys_AddWorkerCmdInternal_t* Sys_AddWorkerCmdInternal = (Sys_AddWorkerCmdInternal_t*)0x005317A0;

void Sys_WaitWorkerCmdInternal(jqWorkerCmd *name);