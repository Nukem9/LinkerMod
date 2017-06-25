#pragma once

enum PROCESS_TYPE
{
	PROCESS_UNKNOWN,
	PROCESS_BLACK_OPS,
	PROCESS_BLACK_OPS_MP,
};

typedef unsigned int processId_t;

int Process_ExecuteConverter(void);

//
// Resolve a process' PROCESS_TYPE from the pid
//
PROCESS_TYPE Process_GetProcessType(processId_t pid);

//
// Searches the active process list for a supported process, and returns the processId
// Returns NULL if no supported process is found withing the given number of ms
//
processId_t Process_FindSupportedProcess(unsigned int timeoutDelay=0);

//
// Suspend all threads for a given processId
//
void Process_SuspendThreads(processId_t pid);

//
// Resume all threads for a given processId
//
void Process_ResumeThreads(processId_t pid);

//
// Kill a given process
//
bool Process_KillProcess(processId_t pid);
