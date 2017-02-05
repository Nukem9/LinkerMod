#define _CRT_SECURE_NO_WARNINGS 1

#include <Windows.h>
#include <iostream>

#ifdef _DEBUG
#define USE_CONSOLE 1
#else
#define USE_CONSOLE 0
#endif

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
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

	return 0;
}