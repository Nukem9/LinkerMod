#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
	#include <Windows.h>
#endif

#include "sys\AppInfo.h"

#include "platform.h"
#include "arg.h"
#include "cvar.h"
#include "cmd.h"

#include "common/io.h"

//
// Sub-main function
//	- run AFTER global setup 
//	- run BEFORE global cleanup
//
int app_main(int argc, char** argv)
{
	if(argc <= 1)
	{
		Arg_PrintUsage();
		return 1;
	}
	
	ArgParsedInfo cmd_info;
	if(int err = Arg_ParseArguments(argc - 1, argv + 1, &cmd_info))
	{
		Con_Error("Fatal Error: %d\n", err);
		return err;
	}

#if _DEBUG
	if (g_dumpCVars.ValueBool())
		CVar::DumpList();
	else
#endif
		Con_Print("\n");
	
	AppInfo_Init();

	return cmd_info.Cmd()->Exec(cmd_info.Argc(), cmd_info.Argv());
}

//
// The true main function:
//	- do any global setup or cleanup here
//
int main(int argc, char** argv)
{
	Con_Init();
	int out = app_main(argc, argv);
	Con_Free();
	return out;
}

//
// Pre-Transition main.cpp
//
#if LEGACY
#include <Windows.h>
#include "AppInfo.h"
#include "cli/arg.h"

#include "common/fs.h"

#include "ff.h"
#include "iwd.h"
#include "process.h"
#include "setup.h"

#include "gdt/assettype\xmodelalias.h"
#include "gdt/assettype\character.h"
#include "gdt/assettype\aitype.h"
#include "gdt/gdt.h"

#include "cmd_ents.h"

GDT gdt;

//
// Used for debugging the GDT regeneration systems
//
int __cdecl fileCallback(const char* filePath, const char* fileName)
{
	Con_Print("%s\n", fileName);

	//XModelAlias* alias = new XModelAlias;
	//alias->ExtractFromGSC(argv[i]);

	//AIType* aitype = new AIType;
	//aitype->ExtractFromGSC(filePath);
	//gdt.assets.push_back(aitype);

	Character* character = new Character;
	character->ExtractFromGSC(filePath);
	gdt.assets.push_back(character);
	return 0;

}

int main(int argc, const char** argv)
{
	//FS_FileIterator("D:\\SteamLibrary\\steamapps\\common\\Call of Duty Black Ops\\raw\\character", "c_rus_spetznaz_rebirth_1*", fileCallback);
	//gdt.WriteFile("test.gdt", GDT_MODE_OVERWRITE);

	if (Arg_HandleArguments(argc, (char**)argv) != NULL)
	{
		return 1;
	}

	if (!AppInfo_Init())
	{
		return 2;
	}

	if (ARG_FLAG_SETUP)
	{
		Setup_Init();
		Setup_Execute();
	}

	bool execPostConvertStep = false;
	if (ARG_FLAG_IMAGE | ARG_FLAG_AUDIO | ARG_FLAG_EVERY)
	{
		FS_FileIterator(AppInfo_IWDDir(), FS_SEARCHPATTERN_IWD, IWD_IWDHandler);
		execPostConvertStep = true;
	}

	if (ARG_FLAG_FF || ARG_FLAG_SND)
	{
		if (ARG_FLAG_LOCALIZED)
		{
			FS_FileIterator(AppInfo_ZoneDir(), FS_SEARCHPATTERN_FF, FF_FFExtract);
		}
		else
		{
			FS_FileIterator(AppInfo_FFDir(), FS_SEARCHPATTERN_FF, FF_FFExtract);
		}

		execPostConvertStep = true;
	}

	if (ARG_FLAG_CONVERT || (execPostConvertStep && ARG_FLAG_SETUP))
	{
		if (Process_ExecuteConverter() != 0)
		{
			Con_Print("ERROR: Converter could not be executed\n");

			if (ARG_FLAG_SETUP)
			{
				Con_Print("\tConverter must be run manually via Launcher to complete the setup\n");
			}

			Con_Print("\n");
		}
	}

	return 0;
}

#endif