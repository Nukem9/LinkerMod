#include "stdafx.h"

void (__cdecl * Com_Init)(char *commandLine);

void Com_ClientPacketEvent()
{
	((void(__cdecl *)())0x0082B290)();
}

void Com_FreeEvent(void *ptr)
{
	Z_Free(ptr, 11);
}

bool Com_RunEventHack()
{
	sysEvent_t event;
	Sys_GetEvent(&event);

	switch (event.evType)
	{
	case SE_NONE:
		Com_ClientPacketEvent();
		return false;
	case SE_KEY:
		CL_KeyEvent(0, event.evValue, event.evValue2, event.evTime);
		return true;
	case SE_CHAR:
		CL_CharEvent(0, event.evValue);
		return true;
	case SE_CONSOLE:
		Cbuf_AddText(0, (char *)event.evPtr);
		Cbuf_AddText(0, "\n");
		Com_FreeEvent(event.evPtr);
		return true;
	}

	Com_Error(ERR_FATAL, "Com_RunEventHack: bad event type %i", event.evType);
	return false;
}

void Com_EventLoop()
{
	// This function might as well be hooked since it's all implemented
	do
	{
		/* Nothing */
	} while (Com_RunEventHack());
}

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

void __cdecl Com_LoadCommonFastFile()
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
		int zoneAllocFlags = (blackopsmode->current.enabled) ? 0x400 : 0x100;

		zoneInfo[zoneCount].name = "common_zombie";
		zoneInfo[zoneCount].allocFlags = zoneAllocFlags;
		zoneInfo[zoneCount++].freeFlags = 0;

		if (fs_gameDirVar && strlen(fs_gameDirVar->current.string) > 0)
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
		zoneInfo[zoneCount].allocFlags = 0x100;
		zoneInfo[zoneCount++].freeFlags = 0;

		//
		// No override is needed for common_patch because common_patch doesn't exist in the vanilla game
		//
		/*if ((*fs_gameDirVar) != NULL && (*fs_gameDirVar)->current.string[0] != NULL)
		{
			if (DB_IsZoneLoaded("common_patch_override"))
				return;

			zoneInfo[zoneCount].name = "common_patch_override";
			zoneInfo[zoneCount].allocFlags = 0x100;
			zoneInfo[zoneCount++].freeFlags = 0;
		}*/
	}

	DB_LoadXAssets(zoneInfo, zoneCount, 0);
}

char *__cdecl Com_GetLevelSharedFastFile(const char *mapName)
{
	char loadBuffer[16384];

	int& gLevelDependenciesInited = *(int*)0x02487BE8;
	int& gLevelDependenciesCount = *(int*)0x02487BE4;
	auto gLevelDependencies = *(char(*)[16][2][64])0x02480F08;

	if (!gLevelDependenciesInited)
	{
		gLevelDependenciesInited = 1;
		const char* buffer = Com_LoadInfoString("level_dependencies.csv", "level_dependency_info", "", loadBuffer);
		Com_BeginParseSession("level_dependencies.csv");
		Com_SetCSV(1);
		while (1)
		{
			const char* token = Com_Parse(&buffer);
			
			if (!*token)
				break;

			if (*token != '/')
			{
				if (gLevelDependenciesCount >= 16)
				{
					Com_PrintWarning(10, "Failed to load level dependencies: Max is %d\n", 16);
				}
				else
				{
					I_strncpyz(gLevelDependencies[gLevelDependenciesCount][0], token, 64);
					token = Com_Parse(&buffer);
					I_strncpyz(gLevelDependencies[gLevelDependenciesCount][1], token, 64);
					gLevelDependenciesCount++;
				}
			}
			else
			{
				//
				// Skip the rest of the line if the token was a comment
				//
				Com_ParseRestOfLine(&buffer);
			}
		}
		Com_EndParseSession();
	}

	for (int i = 0; i < gLevelDependenciesCount; ++i)
	{
		if (!I_strcmp(gLevelDependencies[i][0], mapName))
			return gLevelDependencies[i][1];
	}

	return NULL;
}

void __cdecl Com_LoadLevelFastFiles(const char *mapName)
{
	int zoneCount = 0;
	XZoneInfo zoneInfo[6];

	DB_ResetZoneSize(0);
	UI_SetLoadingScreenMaterial(mapName);

	if (I_stristr(mapName, "zombietron"))
	{
		Dvar_SetBool(zombiemode, 1);
		Dvar_SetBool(zombietron, 1);
	}

	if (!I_strnicmp(mapName, "menu_", 5) || !I_strcmp(mapName, "frontend"))
	{
		zoneInfo[zoneCount].name = "patch_ui";
		zoneInfo[zoneCount].allocFlags = 0x4000000;
		zoneInfo[zoneCount++].freeFlags = 0;
	}

	if (I_strnicmp(mapName, "menu_", 5) && I_strcmp(mapName, "frontend"))
	{
		Com_LoadCommonFastFile();
	}
	
	//
	// Enable the use of level_dependencies.csv
	//
#if _USE_LEVEL_DEPENDENCIES
	char* levelSharedFastFile = Com_GetLevelSharedFastFile(mapName);
	if (levelSharedFastFile)
	{
		// The allocFlags were originally 0x800 - but would cause a free error (these new flags appear to load / unload correctly)
		zoneInfo[zoneCount].name = levelSharedFastFile;
		zoneInfo[zoneCount].allocFlags = 0x4000000;
		zoneInfo[zoneCount++].freeFlags = 0;
	}
#endif

	char specOpsZoneName[64];
	if (!I_strncmp("so_", mapName, strlen("so_")))
	{
		const char* basename = NULL;
		for (basename = &mapName[strlen("so_")]; *basename && *basename != '_'; ++basename) {};

		if (!*basename)
			Com_PrintError(1, "Bad specop level name\n");

		sprintf_s(specOpsZoneName, 64, "%s", basename + 1);
		zoneInfo[zoneCount].name = specOpsZoneName;
		zoneInfo[zoneCount].allocFlags = 0x800;
		zoneInfo[zoneCount++].freeFlags = 0;
	}

	int allocFlags = 0x0;
	if (I_strnicmp(mapName, "menu_", 5) && I_strcmp(mapName, "frontend"))
		allocFlags = I_strncmp("so_", mapName, 3) != 0 ? 0x800 : 0x4000;
	else
		allocFlags = 0x2000000;

	zoneInfo[zoneCount].allocFlags = allocFlags;
	zoneInfo[zoneCount].name = mapName;
	zoneInfo[zoneCount++].freeFlags = 0;

	//
	// Enable <mapname>_patch_override.ff for legacy mod support
	//
	char patchOverrideFastFile[256];
	sprintf_s(patchOverrideFastFile, "%s_patch_override", mapName);
	zoneInfo[zoneCount].name = patchOverrideFastFile;
	zoneInfo[zoneCount].allocFlags = 0x4000000;
	zoneInfo[zoneCount++].freeFlags = 0;

	R_BeginRemoteScreenUpdate();
	DB_LoadXAssets(zoneInfo, zoneCount, 0);
	R_EndRemoteScreenUpdate(NULL);
}
