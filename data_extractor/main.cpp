#include "global.h"
#include <TlHelp32.h>
#include "processHandler.h"

int main(int argc, char** argv)
{
	CreateDirectory(L"techsets",NULL);
	CreateDirectory(L"techniques",NULL);
	CreateDirectory(L"shader_bin",NULL);

	if(DWORD processID = GetSupportedProcess())
	{
		g_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
		CloseHandle(g_hProcess);
	}

	return 0;
};