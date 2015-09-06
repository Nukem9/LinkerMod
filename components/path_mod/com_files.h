#pragma once

typedef int (__cdecl * FS_FOpenFileRead_t)(const char *filename, int *file);
static FS_FOpenFileRead_t FS_FOpenFileRead = (FS_FOpenFileRead_t)0x0054E652;

typedef void (__cdecl * FS_FCloseFile_t)(int h);
static FS_FCloseFile_t FS_FCloseFile = (FS_FCloseFile_t)0x0054C867;

typedef int (__cdecl * FS_Read_t)(void *buffer, int len, int h);
static FS_Read_t FS_Read = (FS_Read_t)0x0054CD6E;

typedef int (__cdecl * FS_ReadFile_t)(const char *qpath, void **buffer);
static FS_ReadFile_t FS_ReadFile_o = (FS_ReadFile_t)0x0054EC65;

#include <string>
static int __cdecl FS_ReadFile(const char *qpath, void **buffer)
{
	int result = FS_ReadFile_o(qpath, buffer);

	//Directory Overrides
	if(result < 0)
	{
		if ( strncmp(qpath, "techsets", strlen("techsets")) == 0 || strncmp(qpath, "techniques", strlen("techniques")) == 0)
		{
			std::string npath = "pimp/";
			npath += qpath;
			
			result = FS_ReadFile_o(npath.c_str(), buffer);
		}
		else
		{
			printf("NOOooooooooooo Coulndt load %s\n",qpath);
		}
	}


	return result;
}

typedef void (__cdecl * FS_FreeFile_t)(void *buffer);
static FS_FreeFile_t FS_FreeFile = (FS_FreeFile_t)0x0054D029;