#include "cmd_common.h"

#include "../sys/process.h"
#include "ripper\process_info.h"

#include "ripper\snd_ripper.h"

int Cmd_Rip_f(int argc, char** argv)
{
	if (processId_t pid = Process_FindSupportedProcess())
	{
		if (int err = ProcessInfo_Init(pid))
		{
			Con_Error("Couldn't initialize process info\n");
			return err;
		}

		std::vector<std::vector<ForeignPointer<snd_snapshot>>> snapshots_table(0);
		DB_EnumAssetPoolEx(ASSET_TYPE_SOUND, Rip_Sound_GatherSnapshots_Callback_f, NULL, &snapshots_table);
		DB_EnumAssetPoolEx(ASSET_TYPE_SOUND, Rip_Sound_Callback_f, NULL, &snapshots_table);

		ProcessInfo_Free();
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