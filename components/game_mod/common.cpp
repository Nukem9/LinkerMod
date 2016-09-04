#include "stdafx.h"
#include <stdarg.h>

void (__cdecl * Com_Init)(char *commandLine);

void hk_Com_Init(char *commandLine)
{
	//
	// Fix up the command line because devs removed it
	//
	commandLine = (char *)0x276D0D8;

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
	XZoneInfo zoneInfo[2];
	int zoneCount = 0;

	DB_ResetZoneSize(0);

	if (useFastFile->current.enabled)
		DB_ReleaseXAssets();

	if (zombietron->current.enabled)
	{
		zoneInfo[zoneCount].name = NULL;
		zoneInfo[zoneCount].allocFlags = 0;
		zoneInfo[zoneCount++].freeFlags = 0x100;

		DB_LoadXAssets(zoneInfo, ++zoneCount, 0);
		return;
	}

	//
	// blackopsmode requires common_zombie for technique data, and common for scripts
	//
	if (zombiemode->current.enabled || blackopsmode->current.enabled)
	{
		if ( DB_IsZoneLoaded("common_zombie") )
			return;

		zoneInfo[zoneCount].name = "common_zombie";
		zoneInfo[zoneCount].allocFlags = 0x100;
		zoneInfo[zoneCount++].freeFlags = 0;
	}
	
	if (!zombiemode->current.enabled)
	{
		if ( DB_IsZoneLoaded("common") )
			return;
		
		zoneInfo[zoneCount].name = "common";
		zoneInfo[zoneCount].allocFlags = 0x100;
		zoneInfo[zoneCount++].freeFlags = 0;
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
		dvar_s* zombiemode = *(dvar_s**)(0x0243FDD4);
		dvar_s* zombietron = *(dvar_s**)(0x0247FDE8);

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
#if _UNSTABLE && _USE_LEVEL_DEPENDENCIES
	char* levelSharedFastFile = Com_GetLevelSharedFastFile(mapName);
	if (levelSharedFastFile)
	{
		zoneInfo[zoneCount].name = levelSharedFastFile;
		zoneInfo[zoneCount].allocFlags = 0x800;
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
