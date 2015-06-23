#include "ArgumentHandler.h"
#include "FileHandler.h"
#include "../D3DBSP_Lib/D3DBSP_Lib.h"

#if _DEBUG
#pragma comment(lib, "../Debug/D3DBSP_Lib.lib")
#else if _RELEASE
#pragma comment(lib, "../Release/D3DBSP_Lib.lib")
#endif

#pragma comment(lib, "Winmm.lib")

int main(int argc, char** argv)
{
	Con_Printf("\n");

	if(argc < 2)
	{
		Con_Printf("Usage: inputfile [arguments]\n\n");
		Con_Printf("Restore Backup:   -restore  OR -r\n");
		Con_Printf("Create Backup:    -backup   OR -b\n");
		Con_Printf("Overwrite Backup: -backup+  OR -b+\n"); 
		Con_Printf("Use Logfile:      -logfile  OR -l\n");
		
		//Con_Printf("Verbose Logfile:  -logfile+ OR -l+\n");
		//Con_Printf("Verbose Messages: -verbose  OR -v\n\n");
		
		return 0;
	}
	else if(argc > 1)
	{
		if(HasArg_Logfile(argc,argv))
			Log_Init("converter.log",LOGFILE_UPDATE_OVERWRITE);

		if(HasArg_Restore(argc,argv))
		{
			int result = File_RestoreBackup(argv[1]);
			return result;
		}

		if(HasArg_Backup(argc,argv))
		{
			File_Backup(argv[1], HasArg_OverwriteBackup(argc,argv));
		}
	
		Con_Printf("Loading D3DBSP File...	");
		DWORD loadStartTime = timeGetTime();
		D3DBSP iBSP;
		if(iBSP.Load(argv[1]) != 0)
		{
			Con_Error("Failed!\n\n");
			return 1;
		}
		Con_Printf("Success!\n\n");
		DWORD loadTime = timeGetTime() - loadStartTime;

		DWORD convertStartTime = timeGetTime();
		D3DBSP oBSP;
		oBSP.ConvertFrom(&iBSP, BSPVERSION_COD_BO);
		DWORD convertTime = timeGetTime() - convertStartTime;

		Con_Printf("Writing D3DBSP File...	");
		DWORD writeStartTime = timeGetTime();
		if(oBSP.Write(argv[1]) != 0)
		{
			Con_Error("Failed!\n\n");
			return 1;
		}
		Con_Printf("Success!\n\n");
		DWORD writeTime = timeGetTime() - writeStartTime;

		Con_Printf("Loaded File In: %d ms\n",loadTime);
		Con_Printf("Converted In: %d ms\n",convertTime);
		Con_Printf("Wrote File In: %d ms\n",writeTime);
	}

	return 0;
}