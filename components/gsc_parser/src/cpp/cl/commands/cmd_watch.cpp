#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
	#include <Windows.h>
#endif

#include "../cl_watch_mode.h"

#include "../../sys/sys_platform.h"
#include "../../sys/sys_worker.h"
#include "../../sys/sys_cpu.h"

#include "../../cache/cache.h"

#include "../cl_arg.h"
#include "../cl_cmd.h"

int Cmd_Watch_f(int argc, char** argv)
{
	CL_WatchMode_Enable();

#ifdef _WIN32
	LARGE_INTEGER freq, start;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);
#else //LINUX
	timespec start;
	clock_gettime(CLOCK_REALTIME, &start);
#endif
	
	int workerCount = Sys_CPUCount() - 1;
	printf("Using %d threads\n", workerCount + 1);
	Worker* workers = new Worker[workerCount];

	for(ssize_t readLen = 0; readLen != -1 && CL_WatchMode_IsEnabled(); )
	{
		size_t bufLen = 64;
		char* buf = (char*)malloc(bufLen);
		
		readLen = getline(&buf, &bufLen, stdin);
		if(readLen > 0)
		{
			//printf("Read %d bytes from stdin '%s'\n", (int)readLen, buf);
			
			ArgParsedInfo cmd_info;
			if(Arg_ParseCmdLine(buf, &cmd_info) != 0)
			{
				free(buf);
				continue;
			}
			
			if(cmd_info.Cmd() && cmd_info.Cmd()->CmdFlags() & COMMAND_WATCH)
			{
				cmd_info.Cmd()->Exec(cmd_info.Argc(), cmd_info.Argv());
			}
			else
			{
				fprintf(stderr, "Error: '%s' is not a valid command in watch mode\n", cmd_info.Cmd()->Name());
			}
		}
		
		free(buf);
	}

	double elapsed_time_ms = 0.0;
#ifdef _WIN32
	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	
	elapsed_time_ms = (double)end.QuadPart - (double)start.QuadPart;
	elapsed_time_ms /= (double)(freq.QuadPart / 1000);
#else //LINUX
	timespec end;
	clock_gettime(CLOCK_REALTIME, &end);

	timespec delta;
	if(end.tv_nsec - start.tv_nsec < 0)
	{
		delta.tv_sec = end.tv_sec - start.tv_sec - 1;
		delta.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
	}
	else
	{
		delta.tv_sec = end.tv_sec - start.tv_sec;
		delta.tv_nsec = end.tv_nsec - start.tv_nsec;
	}
	
	elapsed_time_ms = 1000.0 * (double)delta.tv_sec;
	elapsed_time_ms += (double)delta.tv_nsec / 1000000.0;
	
#endif
	
	Job::PostQuitJob();
	delete[] workers;
	
	Cache_Clear();
	CL_WatchMode_Disable();
	
	printf("Watch mode ended after %f ms\n", elapsed_time_ms);
	
	return 0;
}
