#include "stdafx.h"

void __declspec(naked) hk_R_Cinematic_BinkOpen(void)
{
	_asm
	{
		push[esp + 8]
		push[esp + 8]
		push edi
		call R_Cinematic_BinkOpen
		add esp, 0xC
		retn
	}
}

bool __cdecl R_Cinematic_BinkOpen(const char *filename, unsigned int playbackFlags, char *errText)
{
	char filepath[256] = "\0";

	const char* cwd = Sys_Cwd();
	
	//
	// Attempt to load any custom loadscreens before the stock ones (Allows for mod-specific loadscreens for stock maps)
	//
	const char* fs_game = Dvar_GetString("fs_game");
	if (fs_game || fs_game[0] != '\0')
	{
		_snprintf_s(filepath, 256, "%s\\%s\\video\\%s.%s", cwd, fs_game, filename, "bik");
		if (filepath[0] && R_Cinematic_BinkOpenPath(filepath, playbackFlags, errText))
			return 1;
	}

	if (playbackFlags & 8)
	{
		_snprintf_s(filepath, 256, "bik/%s.%s", filename, "bik");
	}
	else
	{
		_snprintf_s(filepath, 256, "%s\\main\\video\\%s.%s", cwd, filename, "bik");
	}

	if (R_Cinematic_BinkOpenPath(filepath, playbackFlags, errText))
		return 1;

	_snprintf_s(filepath, 256, "%s\\raw\\video\\%s.%s", cwd, filename, "bik");
	if (filepath[0] && R_Cinematic_BinkOpenPath(filepath, playbackFlags, errText))
		return 1;

	return 0;
}

bool __cdecl R_Cinematic_BinkOpenPath(const char *filepath, unsigned int playbackFlags, char *errText)
{
	bool result = 0;

	_asm
	{
		push errText
		push playbackFlags

		mov eax, filepath
		mov ebx, 0x006D96E0
		call ebx
		add esp, 8
		mov result, al
	}

	return result;
}

