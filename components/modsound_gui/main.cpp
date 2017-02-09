#include <crtdbg.h>
#include "common/io.h"
#include <Windows.h>

#include "arg.h"
#include "cvar.h"
#include "cmd.h"

#include "sys\AppInfo.h"

#ifdef _DEBUG
#define USE_CONSOLE 1
#else
#define USE_CONSOLE 0
#endif

int app_main(int argc, char** argv);

//
// The true main function:
//	- do any global setup or cleanup here
//
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int argc = 0;
	char** argv = NULL;

	LPCWSTR cmdLine_w = GetCommandLineW();
	wchar_t** argv_w = CommandLineToArgvW(cmdLine_w, &argc);

	argv = new char*[argc + 1];
	for (int i = 0; i < argc; i++)
	{
		int size = lstrlenW(argv_w[i]) + 1;
		argv[i] = new char[size];
		wcstombs(argv[i], argv_w[i], size);
	}
	argv[argc] = NULL;

	LocalFree(argv_w);

#if USE_CONSOLE
	//
	// Disable STDOUT buffering
	//
	setvbuf(stdout, nullptr, _IONBF, 0);

	//
	// Create an external console
	//
	if (AllocConsole())
	{
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
		freopen("CONIN$", "r", stdin);
	}
#endif

	Con_Init();

#if USE_CONSOLE
	for (int i = 0; i < argc; i++)
	{
		Con_Printf("[%d] %s\n", i, argv[i]);
	}
#endif

	int out = app_main(argc, argv);
	Con_Free();
	
	//
	// Cleanup the command line
	//
	for (int i = 0; i < argc; i++)
	{
		delete[] argv[i];
	}

	delete[] argv;

	_CrtDumpMemoryLeaks();
	return out;
}

//
// Sub-main function
//	- run AFTER global setup 
//	- run BEFORE global cleanup
//
int app_main(int argc, char** argv)
{
	ArgParsedInfo cmd_info;
	if (int err = Arg_ParseArguments(argc - 1, argv + 1, &cmd_info))
	{
		Con_Error("Fatal Error: %d\n", err);
		return err;
	}

	#if _DEBUG
		if (g_dumpCVars.ValueBool())
		{
			CVar::DumpCVars();
			cmd_info.Cmd()->DumpLocalCVars();
		}
		else
	#endif

	AppInfo_Init();
	return cmd_info.Cmd()->Exec(cmd_info.Argc(), cmd_info.Argv());
}
