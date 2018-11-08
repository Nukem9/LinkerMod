// installer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "pe.h"

 void debug_main();

 extern std::string g_data ;
 std::string DownloadReleaseJSon();

extern "C" {
	// module exports file: $(ProjectDir)\exports.def
	int32_t __stdcall TestFunc(char* p)
	{
		// Init g_data
		debug_main();

		char buf[256];
		sprintf_s(buf, "0x%p\n", p);
		// MessageBoxA(0, buf, 0, 0);

		if (!p )
			return g_data.length() + 1;

		strcpy(p, g_data.c_str());
		// debug_main();
		return g_data.length() + 1;
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

	//
	// Get Tags
	//
	Json::Value root;
	std::string g_str = "";

	std::string tags = "";
	// DEPRECATED
	const char* __stdcall LMI_GetTags(void)
	{
		auto j = DownloadReleaseJSon();
	
		Json::Reader reader;

		if (!reader.parse(j, root))
		{
			MessageBoxA(0, "Couldn't parse JSon", 0, 0);
			return 0;
		}


		 g_str = root.toStyledString();
	//	 MessageBoxA(0, g_str.c_str(), 0, 0);

		tags = "";
		for (auto& entry : root)
		{
			tags += entry["name"].asString() + "\r\n";
		}

		return tags.c_str();
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