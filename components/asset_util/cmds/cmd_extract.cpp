#include "../common/ff.h"
#include "../common/iwd.h"

#include "../sys/AppInfo.h"
#include "../common/io.h"

int Cmd_Extract_FF_f(int argc, char** argv)
{
	FS_FileIterator(AppInfo_FFDir(), FS_SEARCHPATTERN_FF, FF_FFExtract);
	return 0;
}

int Cmd_Extract_IWD_f(int argc, char** argv)
{
	FS_FileIterator(AppInfo_IWDDir(), FS_SEARCHPATTERN_IWD, IWD_IWDHandler);
	return 0;
}
