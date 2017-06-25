#include "fs.h"
#include <sys/stat.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
const char fs_dir_separator = '\\';
#else
#include <dirent.h>
const char fs_dir_separator = '/';
#endif

long int FS_FileSize(const char* path)
{
	if(!path)
	{
		return -1;
	}
	
	struct stat _stat;
	if(stat(path, &_stat) == -1)
	{
		return -1;
	}
	
	return _stat.st_size;
}

#ifdef WIN32

long int _FS_Iterate(const char* path, fs_iterator_callback_t file_f, fs_iterator_callback_t dir_f, bool recursive)
{
	WIN32_FIND_DATAA file_data = { NULL };
	HANDLE h_dir;
	if( (h_dir = FindFirstFileA(path, &file_data)) == INVALID_HANDLE_VALUE )
	{
		return 0;
	}
	
	// Saves room on the stack
	char* entry_path = new char[1024];
	
	WIN32_FIND_DATAA entry;
	do
	{
		if( strcmp(entry.cFileName, ".")  == 0 || strcmp(entry.cFileName, "..")  == 0)
		{
			continue;
		}
		
		sprintf_s(entry_path, 1024, "%s%c%s", path, fs_dir_separator, entry.cFileName);
		
		if(entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(dir_f)
			{
				dir_f(entry_path);
			}
			
			if(recursive)
			{
				_FS_Iterate(entry_path, file_f, dir_f, recursive);
			}
		}
		else
		{
			file_f(entry_path);
		}
	} while(FindNextFileA(h_dir, &file_data));
	
	delete[] entry_path;
	
	FindClose(h_dir);
	return 0;
}

#else

long int _FS_Iterate(const char* path, fs_iterator_callback_t file_f, fs_iterator_callback_t dir_f, bool recursive)
{
	DIR* p_dir;
	if( !(p_dir = opendir(path)) )
	{
		return 0;
	}
	
	// Saves room on the stack
	char* entry_path = new char[1024];
	
	struct dirent* entry;
	while((entry = readdir(p_dir)) != NULL)
	{
		if( strcmp(entry->d_name, ".")  == 0 || strcmp(entry->d_name, "..")  == 0)
		{
			continue;
		}
		
		sprintf(entry_path, "%s%c%s", path, fs_dir_separator, entry->d_name);
		
		if(entry->d_type == DT_DIR)
		{
			if(dir_f)
			{
				dir_f(entry_path);
			}
			
			if(recursive)
			{
				_FS_Iterate(entry_path, file_f, dir_f, recursive);
			}
		}
		else
		{
			file_f(entry_path);
		}
	}
	
	delete[] entry_path;
	
	closedir(p_dir);
	return 0;
}

#endif

long int FS_IterateDirectory(const char* path, fs_iterator_callback_t file_callback)
{
	return _FS_Iterate(path, file_callback, NULL, false);
}

long int FS_IterateDirectory(const char* path, fs_iterator_callback_t file_callback, fs_iterator_callback_t dir_callback)
{
	return _FS_Iterate(path, file_callback, dir_callback, false);
}

long int FS_IterateDirectoryTree(const char* path,  fs_iterator_callback_t file_callback)
{
	return _FS_Iterate(path, file_callback, NULL, true);
}

long int FS_IterateDirectoryTree(const char* path, fs_iterator_callback_t file_callback, fs_iterator_callback_t dir_callback)
{
	return _FS_Iterate(path, file_callback, dir_callback, true);
}
