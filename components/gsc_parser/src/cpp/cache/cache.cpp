#include "cache.h"
#include <iostream>

#include "../cl/cl_watch_mode.h"

#include "../util/hash_table.h"
#include "../parser/gsc.yy.h"
#include "../sys/sys_worker.h"

HashTable<ScriptCacheEntry> script_cache;

ScriptCacheEntry* Cache_Update(const char* key)
{
	return script_cache.Add(key);
}

void Cache_Remove(const char* key)
{
	script_cache.RemoveNode(key);
}

void Cache_Clear()
{
	script_cache.Clear();
}

void Cache_List_Callback_f(int index, const char* key, void* value)
{
	ScriptCacheEntry* entry = (ScriptCacheEntry*)value;
	printf("[%d] %s %s\n", index, key, entry ? "VALID" : "NULL");
}

void Cache_List()
{
	int elemCount = script_cache.Traverse(Cache_List_Callback_f);
	printf("%d elements in cache\n", elemCount);
}


ScriptCacheEntry::ScriptCacheEntry(void) : file_data(NULL), file_size(0), ast(NULL)
{
	sem_init(&sem_file, 0, 1);
	sem_init(&sem_ast, 0, 1);
}

ScriptCacheEntry::~ScriptCacheEntry(void)
{
	FlushStreamBuffer();
	FlushAST();
	
	sem_destroy(&sem_ast);
	sem_destroy(&sem_file);
}

void ScriptCacheEntry::LockAST(void)
{
	sem_wait(&sem_ast);
}

void ScriptCacheEntry::LockStreamBuffer(void)
{
	sem_wait(&sem_file);
}

void ScriptCacheEntry::UnlockAST(void)
{
	sem_post(&sem_ast);
}

void ScriptCacheEntry::UnlockStreamBuffer(void)
{
	sem_post(&sem_file);
}

Symbol* ScriptCacheEntry::AST(void) const
{
	return this->ast;
}

size_t ScriptCacheEntry::UpdateStreamBuffer(size_t len, FILE* h)
{
	if(len > file_size)
	{
		delete[] file_data;
		file_data = new char[len];
	}
	
	file_size = len;
	return fread(file_data, 1, file_size, h);
}

//
// The file data is NOT flushed after parsing to allow for other threads
// to refresh it while the AST is being used
//
// arg must be deleted from within this function
//
int ScriptCacheEntry::ParseStreamBuffer(analysis_arg_s* arg)
{
	ScriptCacheEntry* entry = arg->entry;
	
	entry->LockStreamBuffer();
	if(entry->file_data == NULL)
	{
		entry->UnlockStreamBuffer();
		delete arg;
		return 1;
	}
	
	yyscan_t scanner = NULL;
	yylex_init(&scanner);
	
	yy_scan_bytes(entry->file_data, entry->file_size, scanner);
	entry->UnlockStreamBuffer();
	
	Symbol* AST = NULL;
	int err = yyparse(&AST, scanner);
	yylex_destroy(scanner);
	
	//
	// If the new file cannot be parsed
	// the old AST is reused
	//
	if(err)
	{
		delete arg;
		return err;
	}
	
	//
	// Otherwise use the new one
	//
	entry->LockAST();
	Symbol* old = entry->ast;
	entry->ast = AST;
	
	if(arg->callback_func)
	{
		arg->callback_func(AST, arg->callback_argv);
		
		//		
		// Prevents node.js from waiting until the parser exits to handle stdout data
		//
		fflush(stdout);
	}
	
	entry->UnlockAST();
	delete old;
	
	delete arg;
	return 0;
}

void ScriptCacheEntry::FlushStreamBuffer()
{
	LockStreamBuffer();
	file_size = 0;
	delete[] file_data;
	file_data = NULL;
	UnlockStreamBuffer();
}

void ScriptCacheEntry::FlushAST()
{
	LockAST();
	delete ast;
	ast = NULL;
	UnlockAST();
}

int ScriptCacheEntry::PostAnalysisJob(analysis_callback_t callback, void* argv_s)
{	
	//
	// Cleaned up by the ParseStreamBuffer function
	//
	analysis_arg_s* arg = new analysis_arg_s;
	arg->entry = this;
	arg->callback_func = callback;
	arg->callback_argv = argv_s;
	
	if(!CL_WatchMode_IsEnabled())
	{
		ScriptCacheEntry::ParseStreamBuffer(arg);
		return 0;
	} 
	
	Job* job = new Job((job_func_t)ScriptCacheEntry::ParseStreamBuffer, arg);
	job->Register();
	return 0;
}

int ScriptCacheEntry::PostAnalysisJob_Sync(analysis_callback_t callback, void* argv_s)
{	
	//
	// Cleaned up by the ParseStreamBuffer function
	//
	analysis_arg_s* arg = new analysis_arg_s;
	arg->entry = this;
	arg->callback_func = callback;
	arg->callback_argv = argv_s; //TODO: prevent this from leaking
	
	ScriptCacheEntry::ParseStreamBuffer(arg);
	return 0;
}
