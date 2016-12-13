#pragma once

typedef void* (__cdecl* Z_Malloc_t)(size_t size);
static Z_Malloc_t Z_Malloc = (Z_Malloc_t)0x0040354A;

typedef void (__cdecl* Z_Free_t)(void* ptr);
static Z_Free_t Z_Free = (Z_Free_t)0x00402FD7;

void* FS_AllocMem(size_t size);
