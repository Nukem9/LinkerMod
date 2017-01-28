#pragma once

#include <stdio.h>

//
// Returns the size of a given file, or -1 if the file does not exist
//
long int FS_FileSize(const char* path);

typedef long int (*fs_iterator_callback_t)(const char* path);

//
// Iterate through a directory or directory tree
//	file_callback is called for each file
//	dir_callback is called for each directory
//
long int FS_IterateDirectory(const char* path, fs_iterator_callback_t file_callback);
long int FS_IterateDirectory(const char* path, fs_iterator_callback_t file_callback, fs_iterator_callback_t dir_callback);
long int FS_IterateDirectoryTree(const char* path,  fs_iterator_callback_t file_callback);
long int FS_IterateDirectoryTree(const char* path, fs_iterator_callback_t file_callback, fs_iterator_callback_t dir_callback);
