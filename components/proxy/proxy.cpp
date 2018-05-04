#include "stdafx.h"

static std::vector<HMODULE> modules;

struct ProxyMapEntry
{
	const char* process;
	std::vector<const char*> libraries;
};

static ProxyMapEntry proxy_map[] =
{
	{"BlackOps.exe",		{"game_mod.dll"}},
	{"CoDBORadiant.exe",	{"radiant_mod.dll"}},
	{"AssetViewer.exe",		{"asset_viewer.dll"}},
	{"cod2map.exe",			{"cod2map.dll"}},
	{"cod2rad.exe",			{"cod2rad.dll"}},
	{"linker_pc.exe",		{"linker_pc.dll"}}
};

BOOL WINAPI Proxy_Init(HINSTANCE hInst, DWORD reason)
{
	if (reason != DLL_PROCESS_ATTACH)
		return FALSE;

	char name[MAX_PATH];
	GetModuleFileNameA(GetModuleHandleA(NULL), name, _countof(name));

	char *p = strrchr(name, '\\');
	if (!p)
		return FALSE;

	for (int i = 0; name[i]; i++)
	{
		name[i] = p[i + 1];
	}

	for (const auto& entry : proxy_map)
	{
		if (_stricmp(name, entry.process) != 0)
			continue;

		for (const auto& library : entry.libraries)
		{
			HMODULE h = LoadLibraryA(library);
			if (!h)
			{
				MessageBoxA(0, "Failed to load library", 0, 0);
				return FALSE;
			}

			modules.push_back(h);
		}

		break;
	}

	return TRUE;
};

BOOL WINAPI Proxy_Free(HINSTANCE hInst, DWORD reason)
{
	if (reason != DLL_PROCESS_DETACH)
		return FALSE;

	while (modules.size())
	{
		auto h = modules[modules.size() - 1];
		modules.pop_back();

		// Should we add error handling here?
		FreeLibrary(h);
	}
	
	return TRUE;
}
