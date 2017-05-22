#include "stdafx.h"

void __declspec(naked) hk_Scr_ReadFile()
{
	_asm
	{
		push[esp+16]	// archive
		push[esp+16]	// codePos
		push esi		// extFilename
		push[esp+20]	// filename
		push[esp+20]	// inst

		call Scr_ReadFile
		add esp, 20
		
		retn
	}
}

char *__cdecl Scr_ReadFile(scriptInstance_t inst, const char *filename, const char *extFilename, const char *codePos, bool archive)
{
	char* result = nullptr;

	if (!scr_useFastFileOnly->current.integer && (fs_gameDirVar && *fs_gameDirVar->current.string || developer->current.enabled))
	{
		int file = 0;
		if (FS_FOpenFileRead(extFilename, &file) < 0)
		{
			result = Scr_ReadFile_FastFile(inst, filename, extFilename, codePos, archive);
		}
		else
		{
			FS_FCloseFile(file);
			result = Scr_ReadFile_LoadObj(inst, filename, extFilename, codePos, archive);
		}
	}
	else
	{
		Scr_ReadFile_t func = Scr_ReadFile_FastFile;
		if (!useFastFile->current.enabled)
			func = Scr_ReadFile_LoadObj;
		result = func(inst, filename, extFilename, codePos, archive);
	}

	return result;
}

DWORD Scr_ShouldSuppressErrors(void)
{
	static DWORD dwRetn_SuppressError = 0x005A17E9;
	static DWORD dwRetn_HandleError = 0x005A1738;

	if (scr_suppressErrors->current.enabled)
		return dwRetn_SuppressError;
	return dwRetn_HandleError;
}

void __declspec(naked) mfh_RuntimeError(void)
{
	static DWORD dwRetn_SuppressError = 0x005A17E9;
	
	__asm
	{
		jz SUPPRESS_ERROR

		call Scr_ShouldSuppressErrors
		jmp eax

	SUPPRESS_ERROR:
		jmp dwRetn_SuppressError
	}
}
