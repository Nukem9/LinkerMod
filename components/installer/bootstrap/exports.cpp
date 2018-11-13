// installer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "pe.h"
#include "process.h"
#include "ipc.h"

#include <thread>

IPC::Connection app(NULL);

extern "C" {
	int __stdcall IPC_Init(HWND hWnd) {
		app = IPC::Connection(hWnd);
		return 0;
	}

	//
	// Resolve the game's installation path
	// Returns an empty string on failure
	//
	const char* __stdcall LMI_GamePath(void)
	{
		static char game_path[1024] = "";
		GetGameDirectory(game_path, ARRAYSIZE(game_path));
		return game_path;
	}

	//
	// Set the target installation directory & ensure that the install path is valid
	//
	bool __stdcall LMI_SetInstallPath(const char* path)
	{
		const std::string binaryPath = FS::JoinPath(path, "BlackOps.exe");
		return FS::FileExists(binaryPath.c_str());
	}

	int __stdcall LMI_CompareVersions(const char* a, const char* b)
	{
		using namespace semver::v2;
		Version _a(a);
		Version _b(b);

		return Compare(_a, _b);
	}

	//
	// Add a named import to an EXE (if it's not already present)
	//
	int __stdcall PE_AddNamedImport(const char* filepath, const char* moduleName, const char* symbolName) {
		app.printf("Patching imports for '%s'\n", filepath);
		return pe::AddImport(filepath, moduleName, symbolName);
	}

	int __stdcall PE_HasModule(const char* filepath, const char* moduleName) {
		return pe::HasModule(filepath, moduleName);
	}

	int __stdcall PE_HasImport(const char* filepath, const char* moduleName, const char* symbolName) {
		return pe::HasImport(filepath, moduleName, symbolName);
	}	

	//
	// This function is used by the installer
	// to automatically pipe the output from the target command
	// back to the log control in the actual installer window
	//
	int _stdcall LMI_Exec(const Process::LaunchInfo* info)
	{
		assert(info);

		volatile bool terminated = false;

		//
		// TODO: Handle cancel button clicks
		//
		const auto ProcessMessages = []() {
			MSG msg;
			// Use GetMessage() to prevent the loop from going nuts while
			// waiting for messages to show up
			while (GetMessage(&msg, NULL, 0, 0) > 0)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		};

		int retn = 0;
		std::thread worker([&info, &terminated, &retn]() {
			Pipe pipe(true);
			app.printf("Create process... %s\n", info->filename);
			auto process = Process::Create(*info, pipe);

			std::string text = "";
			bool read = true;
			do {
				if (read = pipe.Read(text)) {
					app.print(text);
				}
			} while (read);

			auto retn = process.WaitForExit();
			terminated = true;
			return retn;
		});

		while (!terminated) {
			ProcessMessages();
		}

		worker.join();
		return retn;
	}
}
