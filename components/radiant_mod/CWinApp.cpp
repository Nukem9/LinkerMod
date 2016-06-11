#include "stdafx.h"

typedef void (WINAPI * AfxPostQuitMessage_t)(int nExitCode);
static AfxPostQuitMessage_t AfxPostQuitMessage = (AfxPostQuitMessage_t)0x005BEE73;

typedef int (WINAPI * AfxOleGetUserCtrl_t)();
static AfxOleGetUserCtrl_t AfxOleGetUserCtrl = (AfxOleGetUserCtrl_t)0x005C4783;

typedef void(__thiscall * Map_LoadFile_t)(const char *path);
static Map_LoadFile_t Map_LoadFile = (Map_LoadFile_t)0x0049FF90;

void CWinThread::Run()
{
	((void(__thiscall *)(CWinThread *))0x005BA513)(this);
}

void CWinApp::Run()
{
	if (!*(DWORD *)(this + 32) && AfxOleGetUserCtrl())
		AfxPostQuitMessage(0);

	// Convert the unicode command line to a multi-byte string
	int argc		= 0;
	LPWSTR *argv	= CommandLineToArgvW(GetCommandLineW(), &argc);

	// Open the map specified
	if (argc > 0)
	{
		char path[MAX_PATH];
		wcstombs(path, argv[0], ARRAYSIZE(path));

		Map_LoadFile(path);
	}

	// Free allocated command array
	if (argv)
		LocalFree(argv);

	CWinThread::Run();
}