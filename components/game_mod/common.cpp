#include "stdafx.h"

std::vector<LevelDependency> g_LevelDependencies;

void (__cdecl * Com_Init)(char *commandLine);

void hk_Com_Init(char *commandLine)
{
	// Fix up the command line because devs removed it
	commandLine = (char *)0x276D0D8;

	// Hijack thread index #9 ("Worker7") to use as the console update thread
	Sys_CreateThread(Sys_ConsoleThread, 9);

	// Call original function
	Com_Init(commandLine);
}

void Field_Clear(field_t *edit)
{
	((void(__cdecl *)(field_t *))0x0050DB60)(edit);
}

int nullsub(int arg)
{
	return 0;
}

void Com_ToolPrintf(int channel, const char* fmt, ...)
{
	char msg[4096];

	va_list va;
	va_start(va, fmt);
	_vsnprintf_s(msg, 4096, fmt, va);
	va_end(va);

	Com_Printf(channel, "%s", msg);
	printf("%s", msg);
}

void Com_ToolError(int channel, const char* fmt, ...)
{
	char msg[4096];

	va_list va;
	va_start(va, fmt);
	_vsnprintf_s(msg, 4096, fmt, va);
	va_end(va);

	fprintf(stderr, "%s", msg);
	Com_Error(channel, "%s", msg);
}

bool Com_IsMenuLevel(const char *name)
{
	if (!name)
		name = sv_mapname->current.string;

	return !I_strnicmp(name, "menu_", 5) || !I_strcmp(name, "frontend");
}

void Com_GetLevelSharedFastFiles(const char *mapName)
{
	static bool allDependenciesInit = false;
	static std::vector<LevelDependency> allDependencies;

	if (!allDependenciesInit)
	{
		allDependenciesInit = true;

		char loadBuffer[16384];
		const char* buffer = Com_LoadInfoString("level_dependencies.csv", "level_dependency_info", "", loadBuffer);

		Com_BeginParseSession("level_dependencies.csv");
		Com_SetCSV(true);

		while (true)
		{
			const char* token = Com_Parse(&buffer);
			
			if (!*token)
				break;

			if (*token != '/')
			{
				LevelDependency m;
				memset(&m, 0, sizeof(m));

				strcpy_s(m.base, token);
				token = Com_Parse(&buffer);
				strcpy_s(m.required, token);

				Com_Printf(1, "zone: %s dep: %s\n", m.base, m.required);

				allDependencies.push_back(m);
			}
			else
			{
				// Skip the rest of the line if the token was a comment
				Com_ParseRestOfLine(&buffer);
			}
		}

		Com_EndParseSession();
	}

	// Now return each entry for this map name
	std::vector<LevelDependency> returnedMaps;

	for (auto& dependency : allDependencies)
	{
		if (!_stricmp(dependency.base, mapName))
			returnedMaps.push_back(dependency);
	}

	// Will be empty if there's nothing to load
	g_LevelDependencies = std::move(returnedMaps);
}

void DB_RemoveLevelDependency(const char *dependency)
{
	for (auto itr = g_LevelDependencies.begin(); itr != g_LevelDependencies.end();)
	{
		if (!_stricmp(itr->required, dependency))
			itr = g_LevelDependencies.erase(itr);
		else
			itr++;
	}
}

bool DB_IsLevelDependency(const char *name)
{
	for (const LevelDependency& dep : g_LevelDependencies)
	{
		if (!_stricmp(dep.required, name))
			return true;
	}

	return false;
}

bool DB_IsZoneLoaded(const char *name)
{
	for (int i = 1; i < 33; ++i)
	{
		if (g_zoneNames[i].name[0] && !_stricmp(g_zoneNames[i].name, name))
			return g_zoneNames[i].loaded;
	}

	return false;
}

void Com_LoadCommonFastFile()
{
	XZoneInfo zoneInfo[4];
	int zoneCount = 0;

	DB_ResetZoneSize(0);

	if (useFastFile->current.enabled)
		DB_ReleaseXAssets();

	if (zombietron->current.enabled)
	{
		zoneInfo[zoneCount].name = nullptr;
		zoneInfo[zoneCount].allocFlags = 0;
		zoneInfo[zoneCount++].freeFlags = 0x100;

		DB_LoadXAssets(zoneInfo, zoneCount, 0);
		return;
	}

	//
	// blackopsmode requires common_zombie for technique data, and common for scripts
	//
	if (zombiemode->current.enabled || blackopsmode->current.enabled)
	{
		if (DB_IsZoneLoaded("common_zombie"))
			return;

		// common_zombie must be unloaded after common/en_common
		int zoneAllocFlags = (blackopsmode->current.enabled) ? 0x400 : DB_ZONE_COMMON;

		zoneInfo[zoneCount].name = "common_zombie";
		zoneInfo[zoneCount].allocFlags = zoneAllocFlags;
		zoneInfo[zoneCount++].freeFlags = 0;

		if (strlen(Dvar_GetString("fs_game")) > 0)
		{
			if (DB_IsZoneLoaded("common_zombie_patch_override"))
				return;

			zoneInfo[zoneCount].name = "common_zombie_patch_override";
			zoneInfo[zoneCount].allocFlags = zoneAllocFlags;
			zoneInfo[zoneCount++].freeFlags = 0;
		}
	}

	if (!zombiemode->current.enabled)
	{
		if (DB_IsZoneLoaded("common"))
			return;

		zoneInfo[zoneCount].name = "common";
		zoneInfo[zoneCount].allocFlags = DB_ZONE_COMMON;
		zoneInfo[zoneCount++].freeFlags = 0;

		//
		// No override is needed for common_patch because common_patch doesn't exist in the vanilla game
		//
		/*if (strlen(Dvar_GetString("fs_game")) > 0)
		{
		if (DB_IsZoneLoaded("common_patch_override"))
		return;

		zoneInfo[zoneCount].name = "common_patch_override";
		zoneInfo[zoneCount].allocFlags = DB_ZONE_COMMON;
		zoneInfo[zoneCount++].freeFlags = 0;
		}*/
	}

	if (zoneCount)
	{
		// Don't load map required zones if we already did here
		for (int i = 0; i < zoneCount; i++)
			DB_RemoveLevelDependency(zoneInfo[i].name);

		DB_LoadXAssets(zoneInfo, zoneCount, 0);
	}
}

void Com_LoadLevelFastFiles(const char *mapName)
{
	int zoneCount = 0;
	XZoneInfo zoneInfo[10];

	DB_ResetZoneSize(0);
	UI_SetLoadingScreenMaterial(mapName);
	Com_GetLevelSharedFastFiles(mapName);

	if (I_stristr(mapName, "zombietron"))
	{
		Dvar_SetBool(zombiemode, true);
		Dvar_SetBool(zombietron, true);
	}

	if (Com_IsMenuLevel(mapName))
	{
		zoneInfo[zoneCount].name = "patch_ui";
		zoneInfo[zoneCount].allocFlags = DB_ZONE_FRONTEND;
		zoneInfo[zoneCount++].freeFlags = 0;
	}
	else
	{
		// If we aren't on a menu level, load common assets
		Com_LoadCommonFastFile();
	}
	
	//
	// Enable the use of level_dependencies.csv
	//
#if _USE_LEVEL_DEPENDENCIES
	for (auto& dependency : g_LevelDependencies)
	{
		if (DB_IsZoneLoaded(dependency.required))
			continue;

		// The allocFlags were originally 0x800 - but would cause a free error (these new flags appear to load / unload correctly)
		zoneInfo[zoneCount].name = dependency.required;
		zoneInfo[zoneCount].allocFlags = 0x4000000;
		zoneInfo[zoneCount++].freeFlags = 0;
	}
#endif

	char specOpsZoneName[64];
	if (!I_strncmp("so_", mapName, strlen("so_")))
	{
		const char* basename = nullptr;
		for (basename = &mapName[strlen("so_")]; *basename && *basename != '_'; ++basename)
			/* Do nothing */;

		if (!*basename)
			Com_PrintError(1, "Bad specop level name\n");

		sprintf_s(specOpsZoneName, "%s", basename + 1);
		zoneInfo[zoneCount].name = specOpsZoneName;
		zoneInfo[zoneCount].allocFlags = 0x800;
		zoneInfo[zoneCount++].freeFlags = 0;
	}

	int allocFlags = 0x2000000;

	if (!Com_IsMenuLevel(mapName))
		allocFlags = I_strncmp("so_", mapName, 3) != 0 ? 0x800 : DB_ZONE_LEVEL;

	zoneInfo[zoneCount].allocFlags = allocFlags;
	zoneInfo[zoneCount].name = mapName;
	zoneInfo[zoneCount++].freeFlags = 0;

	//
	// Enable <mapname>_patch_override.ff for legacy mod support
	//
	char patchOverrideFastFile[64];
	sprintf_s(patchOverrideFastFile, "%s_patch_override", mapName);
	zoneInfo[zoneCount].name = patchOverrideFastFile;
	zoneInfo[zoneCount].allocFlags = 0x4000000;
	zoneInfo[zoneCount++].freeFlags = 0;

	R_BeginRemoteScreenUpdate();
	DB_LoadXAssets(zoneInfo, zoneCount, 0);
	R_EndRemoteScreenUpdate(NULL);
}
