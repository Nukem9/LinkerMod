#include <WTypes.h>
#include <iostream>
#include <TlHelp32.h>
#include "processHandler.h"
#include "AssetHandler.h"

int main(int argc, char** argv)
{
	CreateDirectory(L"techsets",NULL);
	CreateDirectory(L"techniques",NULL);
	CreateDirectory(L"shader_bin",NULL);

	if(DWORD processID = GetSupportedProcess())
	{
		g_hProcess = OpenProcess(PROCESS_VM_READ, FALSE, processID);

		s_dumpSettings dumpSettings = {0,0,0,0,0};
		DumpAssets(dumpSettings);

		CloseHandle(g_hProcess);
	}

	return 0;
};