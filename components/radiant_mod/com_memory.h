#pragma once

typedef void* __cdecl Hunk_AllocTempMemory_t(int size);
typedef void __cdecl Hunk_FreeTempMemory_t(void* data);
static Hunk_AllocTempMemory_t* Hunk_AllocTempMemory = (Hunk_AllocTempMemory_t*)0x004C8050;
static Hunk_FreeTempMemory_t* Hunk_FreeTempMemory = (Hunk_FreeTempMemory_t*)0x004C8120;

void *Hunk_Alloc(int size, const char *name, int type);
void Z_Free(void *ptr);
void *Z_Malloc(int size);

void *__cdecl Hunk_FindDataForFile(int type, const char *name);
void *__cdecl Hunk_FindDataForFileInternal(int type, const char *name, int hash);

const char *__cdecl Hunk_SetDataForFile(int type, const char *name, void *data, void *(__cdecl *alloc)(int));
