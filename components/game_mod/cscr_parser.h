#pragma once
#include <WTypes.h>

enum scriptInstance_t : DWORD
{
	SCRIPTINSTANCE_SERVER = 0x0,
	SCRIPTINSTANCE_CLIENT = 0x1,
	SCRIPT_INSTANCE_MAX = 0x2,
};

typedef char *(__cdecl* Scr_ReadFile_t)(scriptInstance_t inst, const char *filename, const char *extFilename, const char *codePos, bool archive);
static Scr_ReadFile_t Scr_ReadFile_FastFile = (Scr_ReadFile_t)0x008A57B0;
static Scr_ReadFile_t Scr_ReadFile_LoadObj = (Scr_ReadFile_t)0x008A58C0;

void hk_Scr_ReadFile();//scriptInstance_t inst, const char *filename, const char *codePos, bool archive);
char *__cdecl Scr_ReadFile(scriptInstance_t inst, const char *filename, const char *extFilename, const char *codePos, bool archive);

void mfh_RuntimeError(void);
