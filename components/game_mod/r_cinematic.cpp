#include "stdafx.h"

void __declspec(naked) hk_R_Cinematic_BinkOpen(void)
{
	__asm
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
	char filepath[256];
	memset(filepath, 0, sizeof(filepath));

	const char* cwd = Sys_Cwd();
	
	//
	// Attempt to load any custom loadscreens before the stock ones (Allows for mod-specific loadscreens for stock maps)
	//
	const char* fs_game = Dvar_GetString("fs_game");
	if (fs_game && fs_game[0] != '\0')
	{
		_snprintf_s(filepath, _TRUNCATE, "%s\\%s\\video\\%s.bik", cwd, fs_game, filename);
		if (R_Cinematic_BinkOpenPath(filepath, playbackFlags, errText))
			return true;
	}

	if (playbackFlags & 8)
		_snprintf_s(filepath, _TRUNCATE, "bik/%s.bik", filename);
	else
		_snprintf_s(filepath, _TRUNCATE, "%s\\main\\video\\%s.bik", cwd, filename);

	if (R_Cinematic_BinkOpenPath(filepath, playbackFlags, errText))
		return true;

	_snprintf_s(filepath, _TRUNCATE, "%s\\raw\\video\\%s.bik", cwd, filename);
	return R_Cinematic_BinkOpenPath(filepath, playbackFlags, errText);
}

bool R_Cinematic_BinkOpenPath(const char *filepath, unsigned int playbackFlags, char *errText)
{
	__asm
	{
		push errText
		push playbackFlags

		mov eax, filepath
		mov ebx, 0x006D96E0
		call ebx
		add esp, 8
	}
}

