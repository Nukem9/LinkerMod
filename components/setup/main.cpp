#include <Windows.h>

#include "arg.h"
#include "steam.h"
#include "files.h"
#include "iwd.h"

int main(int argc, char** argv)
{
	if (Arg_HandleArguments(argc, argv) != NULL)
	{
		return 1;
	}

	if (AppInfo_Init() != 0)
	{
		return 2;
	}


	if (ARG_FLAG_SETUP)
	{
		printf("setup\n");
	}

	if (ARG_FLAG_IMAGE | ARG_FLAG_AUDIO | ARG_FLAG_EVERY)
	{
		FS_FileIterator(AppInfo_IWDDir(), FS_SEARCHPATTERN_IWD, IWD_IWDHandler);
	}
	
	return 0;
}