#include <Windows.h>

#include "../sdk/public/steam/steam_api.h"
#pragma comment(lib, "../sdk/redistributable_bin/steam_api.lib")

#define APPID_BO1_SP	42700
#define APPID_BO1_TOOLS	42740

char appDir_BO1_TOOLS[MAX_PATH] = "";

int main(int argc, char** argv)
{
	if (!SteamAPI_Init())
	{
		SteamAPI_Shutdown();
		printf("ERROR: SteamAPI could not be initialized\n");
		return 1;
	}

	printf("Searching for \"Call of Duty: Black Ops\"...	");
	bool appInstalled_BO1_SP = SteamApps()->BIsAppInstalled(APPID_BO1_SP);
	if (!appInstalled_BO1_SP)
	{
		printf("Failed!\n");
		return 2;
	}
	char appDir_BO1_SP[MAX_PATH] = "";
	SteamApps()->GetAppInstallDir(APPID_BO1_SP, appDir_BO1_SP, MAX_PATH);
	printf("Found!\n	%s\n", appDir_BO1_SP);

	printf("Searching for \"Call of Duty: Black Ops Mod Tools (BETA)\"...	");
	bool appInstalled_BO1_TOOLS = SteamApps()->BIsAppInstalled(APPID_BO1_TOOLS);
	if (!appInstalled_BO1_TOOLS)
	{
		printf("Failed!\n");
		return 2;
	}

	SteamApps()->GetAppInstallDir(APPID_BO1_TOOLS, appDir_BO1_TOOLS, MAX_PATH);
	printf("Found!\n	%s\n", appDir_BO1_TOOLS);

	char buf[MAX_PATH];
	sprintf_s(buf, "%s/main", appDir_BO1_TOOLS);

	SteamAPI_Shutdown();
	return 0;
}