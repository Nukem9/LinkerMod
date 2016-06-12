#pragma once

typedef int (__cdecl FS_ReadFile_t)(const char* qpath, BYTE** buffer);
static FS_ReadFile_t* FS_ReadFile = (FS_ReadFile_t*)0x00418950;

typedef size_t (__cdecl fread_t)(void *, size_t, size_t, FILE *);
static fread_t* cod2map_fread = (fread_t*)0x004C6816;

typedef size_t (__cdecl fwrite_t)(const void *, size_t, size_t, FILE *);
static fwrite_t* cod2map_fwrite = (fwrite_t*)0x004C6990;

typedef int (__cdecl ftell_t)(FILE*);
static ftell_t* cod2map_ftell = (ftell_t*)0x004C62FF;

typedef int (__cdecl fseek_t)(FILE*, int, int);
static fseek_t* cod2map_fseek = (fseek_t*)0x004C60E2;

typedef int (__cdecl fclose_t)(FILE*);
static fclose_t* cod2map_fclose = (fclose_t*)0x004C2E77;