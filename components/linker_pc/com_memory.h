#pragma once

struct HunkUser;

typedef void *(__cdecl * Z_Malloc_t)(int size);
static Z_Malloc_t Z_Malloc = (Z_Malloc_t)0x4C5B20;

typedef void (__cdecl * Z_Free_t)(void *ptr);
static Z_Free_t Z_Free = (Z_Free_t)0x42C7E0;

typedef void *(__cdecl * Hunk_Alloc_t)(int size);
static Hunk_Alloc_t Hunk_Alloc = (Hunk_Alloc_t)0x4C5E00;

typedef void *(__cdecl * Hunk_AllocAlign_t)(int size, int align);
static Hunk_AllocAlign_t Hunk_AllocAlign = (Hunk_AllocAlign_t)0x4C55C0;

typedef void *(__cdecl * Hunk_AllocateTempMemory_t)(int size);
static Hunk_AllocateTempMemory_t Hunk_AllocateTempMemory = (Hunk_AllocateTempMemory_t)0x4C5E80;

typedef bool (__cdecl * Hunk_CheckTempMemoryHighClear_t)();
static Hunk_CheckTempMemoryHighClear_t Hunk_CheckTempMemoryHighClear = (Hunk_CheckTempMemoryHighClear_t)0x4C5330;

typedef void (__cdecl * Hunk_FreeTempMemory_t)(void *buf);
static Hunk_FreeTempMemory_t Hunk_FreeTempMemory = (Hunk_FreeTempMemory_t)0x4C5F90;

typedef void (__cdecl* Hunk_UserDestroy_t)(HunkUser *user);
static Hunk_UserDestroy_t Hunk_UserDestroy = (Hunk_UserDestroy_t)0x004D37A0;
