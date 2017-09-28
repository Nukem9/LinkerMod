#pragma once

#define THREAD_CONTEXT_COUNT 15

struct FastCriticalSection
{
	volatile long readCount;
	volatile long writeCount;
};

void Sys_ConsoleThread(unsigned int index);
void Sys_CreateThread(void(__cdecl * function)(unsigned int index), unsigned int threadContext);
DWORD WINAPI Sys_ThreadMain(LPVOID Arg);
bool Sys_IsMainThread();
bool Sys_IsRenderThread();
void Sys_WakeStream();
void Sys_LockWrite(FastCriticalSection *critSect);
void Sys_UnlockWrite(FastCriticalSection *critSect);