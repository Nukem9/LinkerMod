#include "../common/ff.h"
#include "../common/iwd.h"

#include "../sys/AppInfo.h"
#include "../common/io.h"
#include "../common/fs.h"

#include "../cvar.h"

int Cmd_Extract_FF_f(int argc, char** argv)
{
	if (g_extractAll.ValueBool())
	{
		if (g_useLocalized.ValueBool())
		{
			FS_FileIterator(AppInfo_ZoneDir(), FS_SEARCHPATTERN_FF, FF_FFExtract);
		}
		else
		{
			FS_FileIterator(AppInfo_FFDir(), FS_SEARCHPATTERN_FF, FF_FFExtract);
		}
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			const char* filename = FS_GetFilenameSubString(argv[i]);
			FF_FFExtract(argv[i], filename);
		}
	}
	return 0;
}

int Cmd_Extract_IWD_f(int argc, char** argv)
{
	FS_FileIterator(AppInfo_IWDDir(), FS_SEARCHPATTERN_IWD, IWD_IWDHandler);
	return 0;
}
