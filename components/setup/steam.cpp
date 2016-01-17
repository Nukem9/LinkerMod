#include "steam.h"
#include <Windows.h>
#include "io.h"

static char appDir_BO1[MAX_PATH] = "";

int AppInfo_Init(void)
{
	if (!SteamAPI_Init())
	{
		SteamAPI_Shutdown();
		printf("ERROR: SteamAPI could not be initialized\n");
		return 1;
	}

	printf("\nSearching for \"Call of Duty: Black Ops\"...	");
	bool appInstalled_BO1_SP = SteamApps()->BIsAppInstalled(APPID_BO1_SP);
	if (!appInstalled_BO1_SP)
	{
		printf("Failed!\n");
		return 2;
	}
	char appDir_BO1_SP[MAX_PATH] = "";
	SteamApps()->GetAppInstallDir(APPID_BO1_SP, appDir_BO1_SP, MAX_PATH);
	printf("Found!\n");
	printf_v("	%s\n", appDir_BO1_SP);

	printf("Searching for \"Call of Duty: Black Ops Mod Tools (BETA)\"...	");
	bool appInstalled_BO1_TOOLS = SteamApps()->BIsAppInstalled(APPID_BO1_TOOLS);
	if (!appInstalled_BO1_TOOLS)
	{
		printf("Failed!\n");
		return 2;
	}
	char appDir_BO1_TOOLS[MAX_PATH] = "";
	SteamApps()->GetAppInstallDir(APPID_BO1_TOOLS, appDir_BO1_TOOLS, MAX_PATH);
	printf("Found!\n");
	printf_v("	%s\n\n", appDir_BO1_TOOLS);

	if (strcmp(appDir_BO1_SP, appDir_BO1_TOOLS) != 0)
	{
		printf("ERROR: Directory Mismatch\n");
		return 3;
	}

	strcpy_s(appDir_BO1, appDir_BO1_TOOLS);

	printf_nv("\n");

	SteamAPI_Shutdown();
	return 0;
}

const char* AppInfo_AppDir(void)
{
	return appDir_BO1;
}

const char* AppInfo_FFDir(void)
{
	static char ffCommonDir[MAX_PATH] = "\0";
	if (ffCommonDir[0] == '\0')
	{
		sprintf_s(ffCommonDir, "%s/zone/common", AppInfo_AppDir());
	}

	return ffCommonDir;
}

const char* AppInfo_ZoneDir(void)
{
	static char ffLocalizedDir[MAX_PATH] = "\0";
	if (ffLocalizedDir[0] == '\0')
	{
		sprintf_s(ffLocalizedDir, "%s/zone", AppInfo_AppDir());
	}

	return ffLocalizedDir;
}

const char* AppInfo_IWDDir(void)
{
	static char iwdDir[MAX_PATH] = "\0";
	if (iwdDir[0] == '\0')
	{
		sprintf_s(iwdDir, "%s/main", AppInfo_AppDir());
	}
	
	return iwdDir;
}

const char* AppInfo_RawDir(void)
{
	static char rawDir[MAX_PATH] = "\0";
	if (rawDir[0] == '\0')
	{
		sprintf_s(rawDir, "%s/raw", AppInfo_AppDir());
	}

#if _DEBUG
	return "out/";
#else
	return rawDir;
#endif
}