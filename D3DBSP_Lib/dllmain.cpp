#include "stdafx.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			Con_Init();
			break;
		case DLL_PROCESS_DETACH:
			Con_Restore();
			break;
	}
	
	return TRUE;
}
