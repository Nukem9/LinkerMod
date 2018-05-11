#include "stdafx.h"

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		Proxy_Init(hInst, dwReason);
		break;
	case DLL_PROCESS_DETACH:
		Proxy_Free(hInst, dwReason);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}
