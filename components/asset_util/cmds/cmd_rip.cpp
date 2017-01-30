#include "cmd_common.h"
#include "../sys/process.h"
#include "ripper/process_info.h"

int Cmd_Rip_f(int argc, char** argv)
{
	if (processId_t pid = Process_FindSupportedProcess())
	{
		if (int err = ProcessInfo_Init(pid))
		{
			Con_Error("Couldn't initialize process info\n");
			return err;
		}

		Con_Warning("NOT CURRENTLY IMPLEMENTED - FALLING BACK TO LISTASSETPOOL");

		for (int i = 0; i < ASSET_TYPE_COUNT; i++)
		{
			DB_ListAssetPool((XAssetType)i);
		}

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