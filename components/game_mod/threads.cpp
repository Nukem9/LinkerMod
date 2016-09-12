#include "stdafx.h"

#define THREAD_CONTEXT_COUNT 15

auto threadFunc			= (void(__cdecl **)(unsigned int))0x0251CB44;
HANDLE *threadHandle	= (HANDLE *)0x0251CADC;
DWORD *threadId			= (DWORD *)0x0251CA98;

void Sys_ConsoleThread(unsigned int index)
{
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
	// ASSERT_TRUE(threadFunc[threadContext] == nullptr);
	// ASSERT_TRUE(threadContext < THREAD_CONTEXT_COUNT);

	threadFunc[threadContext]	= function;
	threadHandle[threadContext] = CreateThread(nullptr, 0, Sys_ThreadMain, (LPVOID)threadContext, 0, &threadId[threadContext]);

	// ASSERT_TRUE(threadHandle[threadContext] != nullptr);

	if (!threadHandle[threadContext])
		Com_Printf(1, "Error %d while creating thread %d\n", GetLastError(), threadContext);

	// SetThreadName(threadId[threadContext], s_threadNames[threadContext]);
}

DWORD WINAPI Sys_ThreadMain(LPVOID Arg)
{
	return ((DWORD(WINAPI *)(LPVOID))0x0082FDE0)(Arg);
}