#include "stdafx.h"

auto threadFunc			= (void(__cdecl **)(unsigned int))0x0251CB44;
HANDLE *threadHandle	= (HANDLE *)0x0251CADC;
DWORD *threadId			= (DWORD *)0x0251CA98;

void Sys_ConsoleThread(unsigned int index)
{
	// Loop forever until someone enables this
	while (true)
	{
		if (con_extcon && con_extcon->current.enabled)
			break;

		Sleep(50);
	}

	// Sys_ShowConsole
	((void(__cdecl *)())0x00586100)();

	// Continue handling messages for the window
	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Sys_CreateThread(void(__cdecl * function)(unsigned int index), unsigned int threadContext)
{
	ASSERT(threadFunc[threadContext] == nullptr);
	ASSERT(threadContext < THREAD_CONTEXT_COUNT);

	threadFunc[threadContext]	= function;
	threadHandle[threadContext] = CreateThread(nullptr, 0, Sys_ThreadMain, (LPVOID)threadContext, 0, &threadId[threadContext]);

	ASSERT(threadHandle[threadContext] != nullptr);

	if (!threadHandle[threadContext])
		Com_Printf(1, "Error %d while creating thread %d\n", GetLastError(), threadContext);

	// SetThreadName(threadId[threadContext], s_threadNames[threadContext]);
}

DWORD WINAPI Sys_ThreadMain(LPVOID Arg)
{
	return ((LPTHREAD_START_ROUTINE)0x0082FDE0)(Arg);
}

bool Sys_IsMainThread()
{
	return ((bool(__cdecl *)())0x005A48F0)();
}

bool Sys_IsRenderThread()
{
	return ((bool(__cdecl *)())0x005F5F00)();
}

void Sys_WakeStream()
{
	((void(__cdecl *)())0x00628BD0)();
}

void Sys_LockWrite(FastCriticalSection *critSect)
{
	while (true)
	{
		if (!critSect->readCount)
		{
			if (InterlockedIncrement(&critSect->writeCount) == 1 && !critSect->readCount)
				break;

			InterlockedDecrement(&critSect->writeCount);
		}

		Sleep(0);
	}
}

void Sys_UnlockWrite(FastCriticalSection *critSect)
{
	ASSERT(critSect->writeCount > 0);

	InterlockedDecrement(&critSect->writeCount);
}