#define G_VERSION 1, 0, 0
#include "stdafx.h"

LONG WINAPI MyUnhandledExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo)
{
	//printf("\n\nEXCEPTION DETECTED:\n");
	//printf("EIP: 0x%p\n", ExceptionInfo->ContextRecord->Eip);
	//printf("EX0: 0x%p\n", ExceptionInfo->ExceptionRecord->ExceptionInformation[0]);
	//printf("EX1: 0x%p\n", ExceptionInfo->ExceptionRecord->ExceptionInformation[1]);
	//printf("\n\n");

	//return EXCEPTION_CONTINUE_SEARCH;
	return PageGuard_Check(ExceptionInfo);
}

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

	Light_FixPrimaryLightInfo(&iBSP->lumps[LUMP_PRIMARY_LIGHTS]);
	
	if (Probe_UseDebugSpheres())
		Probe_AddDebugSpheres(iBSP);

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

const int MAX_MAP_COLLISIONVERTS = 65536 * 2;
const int MAX_MAP_COLLISIONVERTS_SIZE = MAX_MAP_COLLISIONVERTS * 12;
BYTE collVertData[MAX_MAP_COLLISIONVERTS_SIZE];
BYTE *collVertDataPtr = (BYTE *)&collVertData;

void Init_MapMod()
{
	if(g_modInitialized)
		return;
	
	setvbuf(stdout, nullptr, _IONBF, 0);
	
	//
	// Logo to let the user know this loaded
	//
	printf("----> Loading custom cod2map\n");
	AddVectoredExceptionHandler(TRUE, (PVECTORED_EXCEPTION_HANDLER)MyUnhandledExceptionFilter);

	//
	// MAX_MAP_COLLISIONVERTS -> Double maximum limit
	//
	PageGuard_Monitor(0x15890C88, 12 * 65536);

	PatchMemory(0x0041ADB2, (PBYTE)&MAX_MAP_COLLISIONVERTS, 4);
	PatchMemory(0x0040AC32, (PBYTE)&MAX_MAP_COLLISIONVERTS_SIZE, 4);
	PatchMemory(0x0040891A, (PBYTE)&MAX_MAP_COLLISIONVERTS_SIZE, 4);
	PatchMemory(0x00408926, (PBYTE)&MAX_MAP_COLLISIONVERTS_SIZE, 4);
	PatchMemory(0x0041ADDD, (PBYTE)&collVertDataPtr, 4);
	PatchMemory(0x0040AC5E, (PBYTE)&collVertDataPtr, 4);
	PatchMemory(0x0040895A, (PBYTE)&collVertDataPtr, 4);

	//
	// Change the file mode for saving d3dbsp files to "wb+"
	//
	PBYTE p_fmode = (PBYTE)&g_fmode;
	PatchMemory(0x00407DE4, p_fmode, sizeof(p_fmode));
	PatchMemory(0x00407E67, p_fmode, sizeof(p_fmode));
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

	//
	// Add Support for Custom KVs
	//
	Detours::X86::DetourFunction((PBYTE)0x0043D649, (PBYTE)&mfh_PrimaryLightHandler);

	//
	// Add -debugReflectionProbes launch arg
	//
	Detours::X86::DetourFunction((PBYTE)0x00406D20, (PBYTE)&Arg_Handle_hk);

	//
	// (Deprecated): Replacement for the pointfile extension
	//
	/*
	void* stringPatch = ".pts";
	PatchMemory(0x0042626F, (PBYTE)&stringPatch, 4);
	PatchMemory(0x00426514, (PBYTE)&stringPatch, 4);
	stringPatch = "%s.pts";
	PatchMemory(0x00406F4E, (PBYTE)&stringPatch, 4);
	*/

	//
	// Increase (double) the max amount of curvenn/terrain collision verts
	//
	int maxCollisionVerts = 131072;
	PatchMemory(0x004197F9, (PBYTE)&maxCollisionVerts, 4);

	Detours::X86::DetourFunction((PBYTE)0x004B3550, (PBYTE)qh_normalize2);

	g_modInitialized = true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call,LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Con_Init();
		Init_MapMod();
		break;
	case DLL_PROCESS_DETACH:
		Con_Restore();
		break;
	}

	return TRUE;
}

