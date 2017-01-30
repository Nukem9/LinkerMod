#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
	#include <Windows.h>
#endif

#include "cmd_common.h"
#include "../cl_arg.h"
#include "../cl_cvar.h"
#include "../cl_cmd.h"

#include "../cl_watch_mode.h"

#include "../../symbols/symbol.h"
#include "../../sys/sys_platform.h"
#include "../../cache/cache.h"
#include "../../fs/fs.h"

#include <stdlib.h>

int Cmd_Tree_ASTCallback_f(Symbol* AST, void* _arg)
{
	AST->PrintInfoRecursive();
	return 0;
}

/*
	USAGE:	tree [filepath]
			tree [filepath fileSize -d fileData]
*/
int Cmd_Tree_f(int argc, char** argv)
{
	if(argc < 2)
	{
		fprintf(stderr, "Error: Incorrect number of arguments\n");
		return 1;
	}
	
	FILE* f = NULL;
	
	ScriptCacheEntry* entry = Cache_Update(argv[1]);
	
	if(argc == 2)
	{
		long int file_size = FS_FileSize(argv[1]);
		if(file_size == -1)
		{
			fprintf(stderr, "Error: File '%s' could not be opened\n", argv[1]);
			return 1;
		}
		
		f = fopen(argv[1], "r");
		if(!f)
		{
			fprintf(stderr, "Error: File '%s' could not be opened\n", argv[1]);
			return 1;
		}
		
		entry->UpdateStreamBuffer(file_size, f);
		fclose(f);
	}
	else if( argc == 3 && CL_WatchMode_IsEnabled() )
	{
		char* end = NULL;
		long int file_size = strtol(argv[2], &end, 10);
		printf("Waiting for %ld bytes on stdin\n", file_size);
		
		entry->UpdateStreamBuffer(file_size, stdin);
	}

	entry->PostAnalysisJob(Cmd_Tree_ASTCallback_f, NULL);
	return 0;
}
