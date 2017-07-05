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

#include "../../util/hash_table.h"

#include <stdlib.h>

//
// Provide all variables defined by assignment expressions
//
void Cmd_Symbols_PrintVars(Symbol* symbol, Position* pos, HashTable<Symbol*>* hash_table)
{
	for(Symbol* node = symbol->Children(); node; node = node->NextElem())
	{
		if(node->Type() == S_TYPE_EXPRESSION && ((Expression*)node)->Operator() == OP_TYPE_ASSIGN)
		{
			Symbol* left_group = node->Children();
			Symbol* left = left_group->Children();
			if(left->Type() == S_TYPE_IDENTIFIER)
			{
				Identifier* id = (Identifier*)left;
				Symbol** s = hash_table->Add(id->value);
				if(*s == NULL)
				{
					*s = id;
					left->PrintSymbol();
				}
			}
		}
		
		Cmd_Symbols_PrintVars(node, pos, hash_table);
	}
}

int Cmd_Symbols_ASTCallback_f(Symbol* AST, void* _arg)
{
	Position* pos = (Position*)_arg;
	
	//
	// Upon deletion this hash table deletes dynamically allocated pointers to symbols
	// but not the symbols themselves
	//
	HashTable<Symbol*>* hash_table = new HashTable<Symbol*>();
	
	for(Symbol* node = AST->Children(); node; node = node->NextElem())
	{
		node->PrintSymbol();
		
		if(pos && node->Type() == S_TYPE_FUNCTION_DECL)
		{
			if(node->Location().start <= *pos && node->Location().end >= *pos)
			{
				// Provide the function arguments
				((Function*)node)->PrintArgs();
				
				// Provide all variables defined within that function
				Cmd_Symbols_PrintVars(node, pos, hash_table);
			}
		}
	}
	
	delete hash_table;
	
	printf("JOB_COMPLETE\n");
	return 0;
}

/*
	USAGE:	symbols [filepath]
			symbols [filepath line:char fileSize fileData] //loc -1:-1 displays all functions in the file
*/
int Cmd_Symbols_f(int argc, char** argv)
{
	if(argc < 2 || argc == 3)
	{
		fprintf(stderr, "Error: Incorrect number of arguments\n");
		return 1;
	}
	
	FILE* f = NULL;
	
	Position* pos = NULL;
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
	else if( argc == 4 && CL_WatchMode_IsEnabled() )
	{
		char* end = NULL;
		
		long int line = strtol(argv[2], &end, 10);
		long int character = strtol(end + 1, &end, 10);
		
		pos = new Position(line, character);
		
		long int file_size = strtol(argv[3], &end, 10);
		
		printf("Waiting for %ld bytes on stdin\n", file_size);
		printf("Using position %ld:%ld\n", line, character);
		
		entry->UpdateStreamBuffer(file_size, stdin);
	}

	entry->PostAnalysisJob_Sync(Cmd_Symbols_ASTCallback_f, pos);
	return 0;
}

//
// PRE EXECUTION TIMER CODE (DEPRECATED)
//
/*#ifdef _WIN32
	LARGE_INTEGER freq, start;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);
#else //LINUX
	timespec start;
	clock_gettime(CLOCK_REALTIME, &start);
#endif*/

//
// POST EXECUTION TIMER CODE (DEPRECATED)
//		
/*
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
	
#endif*/
