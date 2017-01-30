#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>
#include "process.h"
#include "AppInfo.h"
#include "../common/io.h"

#include "../common/fs.h"

#include <Psapi.h>
#pragma comment(lib, "psapi.lib")

int Process_ExecuteConverter(void)
{
	char wd[MAX_PATH];
	sprintf_s(wd, "%s/bin/", AppInfo_AppDir());

	char szCmdLine[MAX_PATH] = "\0";
	sprintf_s(szCmdLine, "%s/converter.exe -nopause", wd);

	PROCESS_INFORMATION processInfo;
	STARTUPINFOA startupInfo;
	memset(&startupInfo, 0, sizeof(STARTUPINFOA));
	startupInfo.cb = sizeof(STARTUPINFOA);

	if (CreateProcessA(NULL, szCmdLine, NULL, NULL, NULL, NULL, NULL, wd, &startupInfo, &processInfo))
	{
		WaitForSingleObject(processInfo.hProcess, INFINITE);
		CloseHandle(processInfo.hThread);
		CloseHandle(processInfo.hProcess);

		return 0;
	}

	return 1;
}

LPCWSTR processStringTable[] =
{
	L"BlackOps.exe",
	//L"BlackOpsMP.exe" - Disabled for VAC potential
};

PROCESS_TYPE Process_GetProcessType(WCHAR* processString)
{
	DWORD supportedProcessCount = sizeof(processStringTable) / sizeof(LPCWSTR);

	for (DWORD i = 0; i < supportedProcessCount; i++)
	{
		if (_wcsicmp(processString, processStringTable[i]) == 0)
		{
			return (PROCESS_TYPE)(i+1);
		}
	}

	return PROCESS_UNKNOWN;
};

PROCESS_TYPE Process_GetProcessType(processId_t pid)
{
	PROCESS_TYPE type = PROCESS_UNKNOWN;

	HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (!handle)
		return type;

	WCHAR buf[MAX_PATH];
	if (GetModuleFileNameExW(handle, 0, buf, MAX_PATH))
	{
		type = Process_GetProcessType((WCHAR*)FS_GetFilenameSubStringW(buf));
	}

	CloseHandle(handle);
	return type;
}

processId_t Process_FindSupportedProcess_Launched(void)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (PROCESS_TYPE processType = Process_GetProcessType(entry.szExeFile))
			{
				CloseHandle(snapshot);

				switch (processType)
				{
				case PROCESS_BLACK_OPS:
				case PROCESS_BLACK_OPS_MP:
					wprintf(L"Supported process found! ('%s')\n", entry.szExeFile);
					return entry.th32ProcessID;
				default:
					return NULL;
				}
			}
		}
	}

	CloseHandle(snapshot);
	return NULL;
}

processId_t Process_FindSupportedProcess(unsigned int timeoutDelay)
{
	_ASSERT(timeoutDelay < UINT_MAX);

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	LARGE_INTEGER start;
	QueryPerformanceCounter(&start);

	if (timeoutDelay > 0)
		Con_Print("Waiting for supported process to launch...\n");

	for (;;)
	{
		LARGE_INTEGER split;
		QueryPerformanceCounter(&split);

		if (processId_t pid = Process_FindSupportedProcess_Launched())
			return pid;

		LARGE_INTEGER elapsed_ms;
		elapsed_ms.QuadPart = split.QuadPart - start.QuadPart;
		elapsed_ms.QuadPart *= 1000;
		elapsed_ms.QuadPart /= freq.QuadPart;

		if (elapsed_ms.QuadPart > timeoutDelay || elapsed_ms.QuadPart > UINT_MAX)
		{
			const char* reason = "";
			if (timeoutDelay)
				reason = " - timeout reached";
			Con_Error("Could not find supported running process%s.\n", reason);
			return NULL;
		}
	}
}

void Process_SuspendThreads(processId_t pid)
{
	HANDLE threadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);

	THREADENTRY32 threadEntry;
	threadEntry.dwSize = sizeof(THREADENTRY32);

	Thread32First(threadSnapshot, &threadEntry);
	do
	{
		if (threadEntry.th32OwnerProcessID == pid)
		{
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);
			SuspendThread(hThread);
			CloseHandle(hThread);
		}
	} while (Thread32Next(threadSnapshot, &threadEntry));

	CloseHandle(threadSnapshot);
}
