#include "cmd_common.h"
#include "../sys/AppInfo.h"

#include "../sys/process.h"
#include "ripper\process_info.h"

#include "ripper\snd_ripper.h"

bool kill_process = true;
bool wait = true;

int Cmd_Rip_f(int argc, char** argv)
{
	processId_t pid = NULL;

	//
	// The user wants to launch a specific map
	// So asset_util handles launching the game
	//
	if (rip_targetMap.ValueString()[0] != '\0')
	{
		rip_waitForMap.Enable();
		rip_waitForProcess.Enable();
		rip_killProcess.Enable();

		if (!AppInfo_MapExists(rip_targetMap.ValueString()))
		{
			Con_Error("ERROR: Map '%s' is not a valid map!\n", rip_targetMap.ValueString());
			return 1;
		}

		char mapname[256];
		const char* prefix = "";

#if _APPINFO_ALLOW_MP_MAPS
		if (_strnicmp(rip_targetMap.ValueString(), "mp_", 3) == 0)
			prefix = "so_dummy_";
#endif
		sprintf_s(mapname, "%s%s", prefix, rip_targetMap.ValueString());

		char cmdLine[1024];
		sprintf_s(cmdLine, "+set g_loadScripts 0 +devmap %s", mapname);

		// TODO figure out a way to do this without the cheaty timer
		pid = Process_LaunchGame(cmdLine);
	}
	else
	{
		//
		// No map was specified, so we wait for the first supported process and use that
		//
		unsigned int timeoutDelay = rip_waitForProcess.ValueBool() ? UINT_MAX : 0;
		pid = Process_FindSupportedProcess(timeoutDelay);
	}

	if (pid)
	{
		if (int err = ProcessInfo_Init(pid))
		{
			Con_Error("Couldn't initialize process info\n");
			return err;
		}

		if (rip_waitForMap.ValueBool())
		{
			if (DB_WaitForMapToLoad() != 0)
			{
				Con_Error("Could not load map - game process is unavailable...\n");
				return 2;
			}
		}

		Process_SuspendThreads(pid);

		snapshots_map_t snapshots_map(0);
		DB_EnumAssetPoolEx(ASSET_TYPE_SOUND, Rip_SoundBank_GatherSnapshots_Callback_f, NULL, &snapshots_map);
		DB_EnumAssetPoolEx(ASSET_TYPE_SOUND, Rip_SoundBank_Callback_f, NULL, &snapshots_map);

		for (auto& set : snapshots_map)
		{
			std::string name = set.first;
			Rip_Snapshot_Callback_f(name, set.second);
		}

		radverbs_map_t radverbs_map(0);
		DB_EnumAssetPoolEx(ASSET_TYPE_SOUND, Rip_SoundBank_GatherRadverbs_Callback_f, NULL, &radverbs_map);

		for (auto& set : radverbs_map)
		{
			std::string name = set.first;
			Rip_Radverb_Callback_f(name, set.second);
		}

		Process_ResumeThreads(pid);

		ProcessInfo_Free();

		if (rip_killProcess.ValueBool())
		{
			Process_KillProcess(pid);
		}

		return 0;
	}

	if (argc < 2)
	{
		char* _argv[] = { NULL, "rip" };
		Cmd_Help_f(ARRAYSIZE(_argv), _argv);
		return -1;
	}

	return 1;
}