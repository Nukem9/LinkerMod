#include "cmd_common.h"

#include "../sys/process.h"
#include "ripper\process_info.h"

#include "ripper\snd_ripper.h"

bool kill_process = true;
bool wait = true;

int Cmd_Rip_f(int argc, char** argv)
{
	unsigned int timeoutDelay = rip_waitForProcess.ValueBool() ? UINT_MAX : 0;
	if (processId_t pid = Process_FindSupportedProcess(timeoutDelay))
	{
		if (int err = ProcessInfo_Init(pid))
		{
			Con_Error("Couldn't initialize process info\n");
			return err;
		}

		if (rip_waitForMap.ValueBool())
		{
			DB_WaitForMapToLoad();
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