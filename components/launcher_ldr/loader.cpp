#include "stdafx.h"

LPVOID GetLoadLibraryAddr()
{
	//
	// TODO: Remote kernel32.dll address can change
	//
	HMODULE hMod = GetModuleHandleA("kernel32.dll");

	return (LPVOID)GetProcAddress(hMod, "LoadLibraryA");
}

HANDLE InjectDll(HANDLE ProcessHandle, const char *Path, const char *Module)
{
	// Allocate memory for the string buffer
	LPVOID memory = VirtualAllocEx(ProcessHandle, nullptr, 0x1000, MEM_COMMIT, PAGE_READWRITE);

	if (!memory)
	{
		printf("Failed to allocate remote process memory\n");
		return nullptr;
	}

	// Write to the remote buffer
	char dllBuffer[MAX_PATH];
	sprintf_s(dllBuffer, "%s\\%s", Path, Module);

	DWORD bytesWritten = 0;
	WriteProcessMemory(ProcessHandle, memory, (LPVOID)&dllBuffer, strlen(dllBuffer) + 1, &bytesWritten);

	// Execute LoadLibrary (thread is suspended)
	HANDLE remoteThread = CreateRemoteThread(ProcessHandle, nullptr, 0, (LPTHREAD_START_ROUTINE)GetLoadLibraryAddr(), memory, CREATE_SUSPENDED, nullptr);

	if (!remoteThread)
	{
		printf("Unable to create remote thread\n");
		return nullptr;
	}

	return remoteThread;
}