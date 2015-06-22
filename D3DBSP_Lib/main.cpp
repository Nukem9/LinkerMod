/*
The MIT License (MIT)

Copyright (c) 2015 SE2Dev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "D3DBSP.h"
#include "ConsoleLog.h"
#include "ArgumentHandler.h"
#include "FileHandler.h"

#pragma comment(lib, "Winmm.lib")

int main(int argc, char** argv)
{
	Con_Init();
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
		
		Con_Restore();
		return 0;
	}
	else if(argc > 1)
	{
		if(HasArg_Logfile(argc,argv))
			Log_Init("converter.log",LOGFILE_UPDATE_OVERWRITE);

		if(HasArg_Restore(argc,argv))
		{
			int result = File_RestoreBackup(argv[1]);
			Con_Restore();
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
			Con_Restore();
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
			Con_Restore();
			return 1;
		}
		Con_Printf("Success!\n\n");
		DWORD writeTime = timeGetTime() - writeStartTime;

		Con_Printf("Loaded File In: %d ms\n",loadTime);
		Con_Printf("Converted In: %d ms\n",convertTime);
		Con_Printf("Wrote File In: %d ms\n",writeTime);
	}

	Con_Restore();
	return 0;
}