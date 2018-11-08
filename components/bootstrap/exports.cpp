// installer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "pe.h"

extern std::string g_data ;
std::string DownloadReleaseJSon();

extern "C" {
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
		return pe::AddImport(filepath, moduleName, symbolName);
	}

	int __stdcall PE_HasModule(const char* filepath, const char* moduleName) {
		return pe::HasModule(filepath, moduleName);
	}

	int __stdcall PE_HasImport(const char* filepath, const char* moduleName, const char* symbolName) {
		return pe::HasImport(filepath, moduleName, symbolName);
	}
}