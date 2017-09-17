#include "stdafx.h"

//
// COMMAND LINE: launcher_ldr DLL EXE ARGUMENTS
//

char g_Directory[MAX_PATH];
char g_ExeDirectory[MAX_PATH];
char g_CommandLine[8192];

HANDLE InjectDll(HANDLE ProcessHandle, const char *Path, const char *Module);

bool StrDel(char* Source, char* Needle, char StopAt)
{
	// Find the location in the string first
	char* loc = strstr(Source, Needle);

	if (!loc)
		return false;

	// "Delete" the word by shifting it over
	int needleLen = strlen(Needle);

	memcpy(loc, loc + needleLen, strlen(loc) - needleLen + 1);

	return true;
}

int StrContainsChar(char* str, char c)
{
	int len = strlen(str);
	for(int i = 0; i < len; i++)
	{
		if(str[i] == c)
			return i;
	}

	return -1;
}

void FixCommandLine(int argc, char *argv[])
{
	// Get the current directory
	GetCurrentDirectoryA(ARRAYSIZE(g_Directory), g_Directory);

	// Skip the first argument, but add the rest
	strcpy_s(g_CommandLine, "\"");
	strcat_s(g_CommandLine, g_Directory);
	strcat_s(g_CommandLine, "\\");
	strcat_s(g_CommandLine, argv[2]);
	strcat_s(g_CommandLine, "\"");

	for(int i = 3; i < argc; i++)
	{
		strcat_s(g_CommandLine, " ");

		// Fix for arguments in quotes being split into multiple args
		char buf[8192];
		if(StrContainsChar(argv[i], ' ') != -1)
		{
			strcpy_s(buf,"");
			strcat_s(buf,"\"");
			strcat_s(buf,argv[i]);
			strcat_s(buf,"\"");
		}
		else
		{
			strcpy_s(buf,argv[i]);
		}

		strcat_s(g_CommandLine, buf);
	}

	// If this is Black Ops, fix the fs_game variable
	if (strstr(g_CommandLine, "BlackOps"))
	{
		StrDel(g_CommandLine, "+set fs_game raw", '\0');

		if (StrDel(g_CommandLine, "+set fs_game dev ", '\0'))
			strcat_s(g_CommandLine, " +set fs_game raw");
	}
}

void FixDirectory(int argc, char *argv[])
{
	// Get the current directory to resolve relative paths
	char temp[MAX_PATH];
	sprintf_s(temp, "%s\\%s", g_Directory, argv[2]);

	char *filePart = nullptr;
	GetFullPathNameA(temp, ARRAYSIZE(temp), g_ExeDirectory, &filePart);

	// Trim off the file name part
	if (filePart)
		*filePart = '\0';
}

void OnErrorOccurred(const char* error, ...)
{
	// Prompt the error and the wait
	va_list varargs;

	va_start(varargs, error);
	vprintf(error, varargs);
	va_end(varargs);

	// Log press any key with windows handler
	printf("\nAn error occurred on launch, press any key to continue...");
	_getch();
}

int main(int argc, char *argv[])
{
	// Disable STDOUT buffering
	setvbuf(stdout, nullptr, _IONBF, 0);

	if (argc < 3)
	{
		OnErrorOccurred("USAGE: launcher_ldr <DLL> <EXE> <ARGUMENTS>\n");
		return 0;
	}

	//
	// Redirect output to this console
	//
	PROCESS_INFORMATION processInfo;
	memset(&processInfo, 0, sizeof(PROCESS_INFORMATION));

	STARTUPINFOA startupInfo;
	memset(&startupInfo, 0, sizeof(STARTUPINFOA));

	// Create a process job object to kill children on exit
	HANDLE ghJob = CreateJobObject(nullptr, nullptr);

	if (ghJob == nullptr)
	{
		OnErrorOccurred("Could not create job object\n");
		return 1;
	}
	else
	{
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION info;
		memset(&info, 0, sizeof(info));

		// Query original information first
		if (!QueryInformationJobObject(ghJob, JobObjectExtendedLimitInformation, &info, sizeof(info), nullptr))
		{
			OnErrorOccurred("Could not QueryInformationJobObject\n");
			return 1;
		}

		//
		// Configure all child processes associated with the job to terminate when the
		// parent process does
		//
		info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

		if (!SetInformationJobObject(ghJob, JobObjectExtendedLimitInformation, &info, sizeof(info)))
		{
			OnErrorOccurred("Could not SetInformationJobObject\n");
			return 1;
		}
	}

	// Call the real process
	FixCommandLine(argc, argv);
	FixDirectory(argc, argv);

	if(!CreateProcessA(nullptr, g_CommandLine, nullptr, nullptr, TRUE, CREATE_BREAKAWAY_FROM_JOB | CREATE_SUSPENDED, nullptr, g_ExeDirectory, &startupInfo, &processInfo))
	{
		OnErrorOccurred("Failed to create '%s' process\n", argv[2]);
		return 1;
	}

	// Assign the job object
	if (!AssignProcessToJobObject(ghJob, processInfo.hProcess))
	{
		OnErrorOccurred("Unable to assign child process job object (0x%X)\n", GetLastError());
		return 1;
	}

	// Inject the DLL
	HANDLE injectThread = InjectDll(processInfo.hProcess, g_Directory, argv[1]);

	if (!injectThread)
	{
		OnErrorOccurred("DLL injection failed\n");
		return 1;
	}

	// Resume injection thread, then the process
	ResumeThread(injectThread);
	WaitForSingleObject(injectThread, INFINITE);

	ResumeThread(processInfo.hThread);

	CloseHandle(processInfo.hThread);
	CloseHandle(injectThread);

	// Clear the console window, we are finished with it
	FreeConsole();

	// Wait until it exits
	WaitForSingleObject(processInfo.hProcess, INFINITE);

	// Determine the real process exit code
	DWORD exitCode = 0;
	GetExitCodeProcess(processInfo.hProcess, &exitCode);

	CloseHandle(processInfo.hProcess);
	CloseHandle(ghJob);

	// Send the exit code to the caller
	ExitProcess(exitCode);
	return exitCode;
}
