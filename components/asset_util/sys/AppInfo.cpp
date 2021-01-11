#include <Windows.h>
#include "../setup.h"
#include "AppInfo.h"
#include "../common/io.h"
#include "../common/fs.h"

#include "../cvar.h"

char g_GameDirectory[MAX_PATH];

bool AppInfo_Init()
{
	const char* _cli_gamepath = fs_gamepath.ValueString();
	if (_cli_gamepath != nullptr && *_cli_gamepath != '\0') {
		// TODO - Validate that the path actually contains the game
		strcpy_s(g_GameDirectory, _cli_gamepath);
		return true;
	}

	const bool foundPath = GetGameDirectory(g_GameDirectory, ARRAYSIZE(g_GameDirectory));
	if(!foundPath)
	{
		Con_Error("ERROR: Unable to locate game directory!\n");
#if PRODUCTION && 0
		Con_Print("Press <Return> to Quit");
		fflush(stdout);
		getchar();
#endif
	}
	else
	{
		Con_Print_v("GAME_DIR: \"%s\"\n\n", g_GameDirectory);
	}

	return foundPath;
}

const char* AppInfo_AppDir()
{
	return g_GameDirectory;
}

const char* AppInfo_FFDir()
{
	static char ffCommonDir[MAX_PATH];

	if (ffCommonDir[0] == '\0')
		sprintf_s(ffCommonDir, "%s/zone/common", AppInfo_AppDir());

	return ffCommonDir;
}

const char* AppInfo_ZoneDir()
{
	static char ffLocalizedDir[MAX_PATH];

	if (ffLocalizedDir[0] == '\0')
		sprintf_s(ffLocalizedDir, "%s/zone", AppInfo_AppDir());

	return ffLocalizedDir;
}

const char* AppInfo_IWDDir()
{
	static char iwdDir[MAX_PATH];

	if (iwdDir[0] == '\0')
		sprintf_s(iwdDir, "%s/main", AppInfo_AppDir());

	return iwdDir;
}

const char* AppInfo_RawDir()
{
#if _DEBUG
	return "out/";
#else
	static char rawDir[MAX_PATH];

	if (rawDir[0] == '\0')
		sprintf_s(rawDir, "%s/raw", AppInfo_AppDir());

	return rawDir;
#endif
}

const char* AppInfo_OutDir()
{
	if (fs_outdir.ValueString()[0] == '\0')
		return AppInfo_RawDir();

	if (strchr(fs_outdir.ValueString(), ':') != NULL && strchr(fs_outdir.ValueString(), ':') != fs_outdir.ValueString() + 1)
	{
		Con_Warning("Invalid output directory - falling back to default\n");
		fs_outdir.AssignRawString("");
	}

	return fs_outdir.ValueString();
}

bool AppInfo_MapExists(const char* mapname)
{
	const char* whitelist[] =
	{
		"frontend",

		// Campaign
		"cuba",
		"vorkuta",
		"pentagon",
		"flashpoint",
		"khe_sanh",
		"hue_city",
		"kowloon",
		"fullahead",
		"creek_1",
		"river",
		"wmd_sr71",
		"wmd",
		"pow",
		"rebirth",
		"int_escape",
		"underwaterbase",
		"outro",

		// Narrative
		"so_narrative1_frontend",
		"so_narrative2_frontend",
		"so_narrative3_frontend",
		"so_narrative4_frontend",
		"so_narrative5_frontend",
		"so_narrative6_frontend",

		// Zombies
		"zombie_theater",
		"zombie_pentagon",
		"zombietron",
		"zombie_cod5_prototype",	// DLC1, DLC5
		"zombie_cod5_asylum",		// DLC1, DLC5
		"zombie_cod5_sumpf",		// DLC1, DLC5
		"zombie_cod5_factory",		// DLC1, DLC5
		"zombie_cosmodrome",		// DLC2
		"zombie_coast",				// DLC3
		"zombie_temple",			// DLC4
		"zombie_moon",				// DLC5

#if _APPINFO_ALLOW_MP_MAPS
		// Multiplayer Maps
		"mp_array",
		"mp_cairo",
		"mp_cosmodrome",
		"mp_cracked",
		"mp_crisis",
		"mp_duga",
		"mp_firingrange",
		"mp_hanoi",
		"mp_havoc",
		"mp_mountain",
		"mp_nuked",
		"mp_radiation",
		"mp_russianbase",
		"mp_villa",

		// DLC2
		"mp_berlinwall2",
		"mp_discovery",
		"mp_kowloon",
		"mp_stadium",

		//DLC3
		"mp_gridlock",
		"mp_hotel",
		"mp_outskirts",
		"mp_zoo",

		// DLC4
		"mp_area51",
		"mp_drivein",
		"mp_golfcourse",
		"mp_silo",
#endif
	};

	// Ensure the map is whitelisted
	bool isValidMap = false;
	for (int i = 0; i < ARRAYSIZE(whitelist); i++)
	{
		if (_stricmp(mapname, whitelist[i]) == 0)
		{
			isValidMap = true;
			break;
		}
	}

	if (!isValidMap)
		return false;

	// Check if the map's fastfile is present
	char path[1024];
	sprintf_s(path, "%s\\%s.ff", AppInfo_FFDir(), mapname);
	return FS_FileExists(path);
}
