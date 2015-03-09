#include "processHandler.h"
#include <TlHelp32.h>

HANDLE g_hProcess;

BYTE* db_hashTable;
XAssetEntryPoolEntry* g_assetEntryPool;
XZoneName* g_zoneNames;

LPCWSTR processStringTable[] = 
{
	L"BlackOps.exe",
	L"BlackOpsMP.exe"
};

PROCESS_TYPE IsSupportedProcess(WCHAR* processString)
{
	DWORD supportedProcessCount = sizeof(processStringTable)/sizeof(LPCWSTR);

	for(DWORD i = 0; i < supportedProcessCount; i++)
	{
		if(_wcsicmp(processString, processStringTable[i]))
		{
			return (PROCESS_TYPE)i;
		}
	}
	return PROCESS_UNKNOWN;
};

DWORD GetSupportedProcess()
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if(PROCESS_TYPE processType = IsSupportedProcess(entry.szExeFile))
			{
				CloseHandle(snapshot);

				switch (processType)
				{
				case PROCESS_BLACK_OPS:
					db_hashTable = (BYTE*)0x00CD81F8;
					g_assetEntryPool = (XAssetEntryPoolEntry*)0x00DE85D8;
					g_zoneNames = (XZoneName*)0x010C6608;
					return entry.th32ProcessID;
				case PROCESS_BLACK_OPS_MP:
					db_hashTable = (BYTE*)0x24365D8;
					g_assetEntryPool = (XAssetEntryPoolEntry*)0x252FC18;
					g_zoneNames = (XZoneName*)0x028C9B08;
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