#include <Windows.h>
#include "../shared/utility.h"
#include "../shared/detours/Detours.h"

#include "com_files.h"

int __cdecl hk_FS_ReadFile(const char* path, BYTE** fileData)
{
	int result = FS_ReadFile(path,fileData);
	
	if(StrEndsWith(path,".d3dbsp"))
	{
		D3DBSP iBSP;
		iBSP.Load(*fileData);
		iBSP.Convert(BSPVERSION_COD_WAW);
		iBSP.Write(*fileData);
		result = iBSP.PotentialFileSize();
	}

	return result;
}

int __cdecl ConvertBSP_Post(FILE* h)
{
	long len = cod2map_ftell(h);
	BYTE* buf = new BYTE[len];

	cod2map_fseek(h, 0, SEEK_SET);
	cod2map_fread(buf, 1, len, h);

	D3DBSP* iBSP = new D3DBSP;
	iBSP->Load(buf);
	delete[] buf;

	iBSP->Convert(BSPVERSION_COD_BO);
	len = iBSP->PotentialFileSize();
	buf = new BYTE[len];
	iBSP->Write(buf);
	delete iBSP;

	cod2map_fseek(h, 0, SEEK_SET);
	cod2map_fwrite(buf, 1, len, h);

	return cod2map_fclose(h);
}

void* retLoc_ConvertBSP_Update_Post = (void*)0x00409483;
void __declspec(naked) hk_ConvertBSP_Update_Post()
{
	_asm
	{
		call ConvertBSP_Post
		mov ecx, [esp + 0x18]
		jmp retLoc_ConvertBSP_Update_Post
	}
}

void* retLoc_ConvertBSP_Post = (void*)0x00409511;
void __declspec(naked) hk_ConvertBSP_Post()
{
	_asm
	{
		call ConvertBSP_Post
		add esp, 4
		jmp retLoc_ConvertBSP_Post
	}
}

static char* g_fmode = "wb+";
bool g_modInitialized = false;

void Init_MapMod()
{
	if(g_modInitialized)
		return;
	
	setvbuf(stdout, nullptr, _IONBF, 0);
	
	//
	// Change the file mode for saving d3dbsp files to "wb+"
	//
	PBYTE p_fmode = (PBYTE)&g_fmode;
	PatchMemory(0x00407DE4, p_fmode, sizeof(p_fmode));
	PatchMemory(0x00407E65, p_fmode, sizeof(p_fmode));
	PatchMemory(0x00407EAF, p_fmode, sizeof(p_fmode));
	PatchMemory(0x00407EE6, p_fmode, sizeof(p_fmode));

	//
	// FS_ReadFile Hook which - when passed a d3dbsp file - enforces bsp version before passing to cod2map
	//
	FS_ReadFile = (FS_ReadFile_t*)Detours::X86::DetourFunction((PBYTE)0x00418950, (PBYTE)hk_FS_ReadFile);

	//
	// Com_SaveBSP Hooks which enforce bsp version before saving
	//
	Detours::X86::DetourFunction((PBYTE)0x00409509, (PBYTE)&hk_ConvertBSP_Post); 
	Detours::X86::DetourFunction((PBYTE)0x0040947A, (PBYTE)&hk_ConvertBSP_Update_Post);

	g_modInitialized = true;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call,LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Init_MapMod();
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

