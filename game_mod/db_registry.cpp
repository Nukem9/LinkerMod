#include "stdafx.h"

void DB_ModXFileHandle(HANDLE *zoneFile, char* zoneName, FF_DIR *zoneDir)
{
	printf("DB_MoxXFileHandle\n");
	dvar_s** fs_gameDirVar = (dvar_s**)0x025FADE8;
	dvar_s** fs_usermapDir = (dvar_s**)0x025FADE4;
	dvar_s** fs_homepath = (dvar_s**)0x025FBF0C;
	char* str_mod = (char*)0x00A18DE8;

	HANDLE h = 0;
	char filename[MAX_PATH];
	
	if( (*fs_gameDirVar) && (*fs_gameDirVar)->current.string && !strcmp(zoneName,str_mod)) //Loads mods/fs_game/mod.ff
	{
		sprintf_s(filename, "%s\\%s\\%s.ff", (*fs_homepath)->current.string, (*fs_gameDirVar)->current.string, zoneName);
		HANDLE h = CreateFileA(filename, 0x80000000, 1u, 0, 3u, 0x60000000u, 0);
		*zoneFile = h;
		if ( h != (HANDLE)-1 )
			*zoneDir = FFD_MOD_DIR;
	}
	else if(strcmp((*fs_gameDirVar)->current.string, "raw") != 0 && strlen((*fs_gameDirVar)->current.string) > 0) //Load custom maps associated with mods
	{
		sprintf_s(filename,"%s\\%s\\%s.ff", (*fs_homepath)->current.string, (*fs_gameDirVar)->current.string, zoneName);
	
		h = CreateFileA(filename, 0x80000000, 1u, 0, 3u, 0x60000000u, 0);
		*zoneFile = h;
		if(h != (HANDLE)-1)
			*zoneDir = FFD_MOD_DIR;
	}
	else //Load usermaps/map
	{
		//h = (HANDLE)(*fs_usermapDir);
		if( (*fs_usermapDir) && (*fs_usermapDir)->current.string)
		{
			const char* usermapDir = (*fs_usermapDir)->current.string;
			sprintf_s(filename, "%s\\%s\\%s\\%s.ff", (*fs_homepath)->current.string, "usermaps", usermapDir, zoneName);

			h = CreateFileA(filename, 0x80000000, 1u, 0, 3u, 0x60000000u, 0);
			*zoneFile = h;
			if(h != (HANDLE)-1)
				*zoneDir = FFD_USER_MAP;
		}
	}
}

void __declspec(naked) DB_ModXFileHandle_hk()
{
	__asm
	{
		push edx //zoneDir
		push esi //zoneName
		push edi //zoneFile
		call DB_ModXFileHandle
		add esp, 12
		retn
	}
}