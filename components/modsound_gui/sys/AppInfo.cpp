#include <Windows.h>
#include "../setup.h"
#include "AppInfo.h"
#include "../common/io.h"
#include "../common/fs.h"

#include "../cvar.h"

char g_GameDirectory[MAX_PATH];

bool AppInfo_Init()
{
	bool foundPath = GetGameDirectory(g_GameDirectory, ARRAYSIZE(g_GameDirectory));

	if(foundPath)
		Con_Printf_v("GAME_DIR: \"%s\"\n", g_GameDirectory);
	else
		Con_Warning_v("GAME_DIR: \"%s\"\n", "NOT_FOUND");
	Con_Printf_v("\n");

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
	if (strlen(fs_outdir.ValueString()) == 0)
		return AppInfo_RawDir();

	if (strstr(fs_outdir.ValueString(), ":") != NULL && strstr(fs_outdir.ValueString(), ":") != fs_outdir.ValueString() + 1)
	{
		Con_Warning("Invalid output directory - falling back to default\n");
		fs_outdir.AssignRawString("");
	}

	return fs_outdir.ValueString();
}