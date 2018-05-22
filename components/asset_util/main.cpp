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

#if _LEGACY
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
#endif

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
	{
		CVar::DumpCVars();
		cmd_info.Cmd()->DumpLocalCVars();
	}
	else
#endif

	// Attempt to initialize the app info...
	// quit if we were unable to do so
	if(!AppInfo_Init())
		return 1;

	// Execute the requested command
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
