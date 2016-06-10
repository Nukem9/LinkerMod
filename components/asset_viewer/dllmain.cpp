#include "stdafx.h"

void strcpy_safe(char *Dest, const char *Src)
{
	size_t srcLen = strlen(Src);
	size_t dstLen = strlen(Dest);

	ASSERT(srcLen <= dstLen);

	DWORD d;
	VirtualProtect(Dest, srcLen, PAGE_EXECUTE_READWRITE, &d);
	strcpy_s(Dest, dstLen, Src);
	VirtualProtect(Dest, srcLen, d, &d);
}

bool g_Initted = false;

BOOL AssetViewerMod_Init()
{
	if (g_Initted)
		return FALSE;

	//
	// Disable STDOUT buffering
	//
	setvbuf(stdout, nullptr, _IONBF, 0);

	//
	// Create an external console for AssetViewer
	//
	if (AllocConsole())
	{
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
		freopen("CONIN$", "r", stdin);
	}

	//
	// Fix writing model pics to wrong directory
	//
	const char* model_pics_path = "..\\docs\\model_pics\\";
	ASSERT(strlen(model_pics_path) < 24);
	PatchMemory(0x009471EC, (PBYTE)model_pics_path, strlen(model_pics_path) + 1);

	//
	// Disable the "fileSize > 0" assertion for materials
	//
#if ASSET_VIEWER_DISABLE_MATERIAL_ASSERT
	PatchMemory_WithNOP(0x0080480C, 5);
	PatchMemory_WithNOP(0x0080481B, 1);
#endif

	g_Initted = true;

	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		return AssetViewerMod_Init();
	}

	return TRUE;
}
