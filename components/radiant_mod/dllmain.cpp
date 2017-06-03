#define G_VERSION 1, 0, 0
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

void __declspec(naked) hk_Com_Printf()
{
	__asm
	{
		pushad
		push ebp
		mov ebp, esp

		push [ebp + 0x18 + 0x20]
		push [ebp + 0x14 + 0x20]
		push [ebp + 0x10 + 0x20]
		push [ebp + 0xC + 0x20]
		; push [ebp + 0x8 + 0x20] (int level)
		call printf
		add esp, 0x10

		pop ebp
		popad
		retn
	}
}

BOOL RadiantMod_Init()
{
	//
	// Disable STDOUT buffering
	//
	setvbuf(stdout, nullptr, _IONBF, 0);

	//
	// Create an external console for Radiant
	//
	if (AllocConsole())
	{
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
		freopen("CONIN$", "r", stdin);
	}

#if USE_NSIGHT_FIX
	if (PatchNvidiaTools())
	{
		// Force windows to always be redrawn
		PatchMemory_WithNOP(0x0042EDC4, 10);
		*(DWORD *)0x027C32F4 = 0xFFFFFFFF;
	}
#endif

	//
	// Use D3D9Ex when available to prevent lost devices
	//
	Detours::X86::DetourFunction((PBYTE)0x0051B75A, (PBYTE)&hk_Direct3DCreate9, Detours::X86Option::USE_CALL);
	Detours::X86::DetourFunction((PBYTE)0x0051B5CC, (PBYTE)&hk_CreateDevice, Detours::X86Option::USE_CALL);
	Detours::X86::DetourFunction((PBYTE)0x0054F34F, (PBYTE)&hk_GetSwapChain, Detours::X86Option::USE_CALL);
	Detours::X86::DetourFunction((PBYTE)0x0051B891, (PBYTE)&hk_CreateAdditionalSwapChain, Detours::X86Option::USE_CALL);
	Detours::X86::DetourFunction((PBYTE)0x0051C25E, (PBYTE)&hk_CreateAdditionalSwapChain, Detours::X86Option::USE_CALL);
	Detours::X86::DetourFunction((PBYTE)0x0056AC60, (PBYTE)&Image_Setup);
	PatchMemory(0x0053E492, (PBYTE)"\xEB", 1); // (Present() patch when focus is lost)

	//
	// Redirect all resource (resx) loading to this dll first
	//
	void *ptr = hk_LoadResource;
	PatchMemory(0x006414F0, (PBYTE)&ptr, 4);

	ptr = hk_SizeofResource;
	PatchMemory(0x006414F8, (PBYTE)&ptr, 4);

	ptr = hk_FindResourceA;
	PatchMemory(0x006414EC, (PBYTE)&ptr, 4);

	ptr = hk_LoadMenuA;
	PatchMemory(0x0064183C, (PBYTE)&ptr, 4);

	ptr = hk_LoadCursorA;
	PatchMemory(0x006418B4, (PBYTE)&ptr, 4);

	ptr = hk_LoadAcceleratorsA;
	PatchMemory(0x00641678, (PBYTE)&ptr, 4);

	ptr = hk_LoadIconA;
	PatchMemory(0x0064180C, (PBYTE)&ptr, 4);

	ptr = hk_LoadBitmapA;
	PatchMemory(0x00641828, (PBYTE)&ptr, 4);

	//
	// Enable com_printf again
	//
	FixupFunction(0x004683F0, (ULONG_PTR)&hk_Com_Printf);

	//
	// Hook CWinApp::Run to allow for automatic map loading via command line arguments
	//
	//Detours::X86::DetourFunctionClass((PBYTE)0x005BF26E, &CWinApp::Run);

	//
	// FS_ReadFile Hook - Set Up Fallback Location for Techsets and Techniques
	//
	o_FS_ReadFile = (FS_ReadFile_t)Detours::X86::DetourFunction((PBYTE)0x004BC840, (PBYTE)FS_ReadFile);

	//
	// Image loading
	//
	Detours::X86::DetourFunction((PBYTE)0x0052BE30, (PBYTE)&Material_ReloadTextures);
	Detours::X86::DetourFunction((PBYTE)0x005675B0, (PBYTE)&hk_Image_LoadFromFileWithReader);

#if WAW_PIMP
	//
	// Load techset override data
	//
	FS_Init_TechsetOverride();
#endif

	//
	// Hook shader/technique/techset loading functions for PIMP (ShaderWorks)
	//
#if !WAW_PIMP
	Detours::X86::DetourFunction((PBYTE)0x00530550, (PBYTE)&hk_Material_LoadPass);
	Detours::X86::DetourFunction((PBYTE)0x0052F700, (PBYTE)&hk_Material_LoadShader);
#endif
	Detours::X86::DetourFunction((PBYTE)0x00530D60, (PBYTE)&Material_LoadTechniqueSet);

	//
	// Hooks for Techset remapping of broken materials
	//
	Detours::X86::DetourFunction((PBYTE)0x00532AA6, (PBYTE)&mfh_MaterialLoad);
	o_Material_LoadRaw = (Material_LoadRaw_t*)Detours::X86::DetourFunction((PBYTE)0x005325F0, (PBYTE)&Material_LoadRaw);

	//
	// Hook Xmodel loading functions to support Black Ops
	//
	Detours::X86::DetourFunction((PBYTE)0x004DFAA0, (PBYTE)&hk_XModelLoadConfigFile);

	PatchMemory(0x004E09DB, (PBYTE)"\xEB", 1);// Xmodelsurfs version check
	PatchMemory(0x004DF7F6, (PBYTE)"\xEB", 1);// Xmodelparts version check
	Detours::X86::DetourFunction((PBYTE)0x0053519E, (PBYTE)&mfh_XModelReadSurface); // 4 byte xmodelsurfs file adjustment (MagicNumber)

	//
	// Re-brand CoDWAWRadiant to CoDBORadiant
	//
	strcpy_safe((char *)0x006F8688, "Software\\iw\\CoDBORadiantModTool\\CoDBORadiantModTool");
	strcpy_safe((char *)0x006F0CD0, "Software\\iw\\CoDBORadiantModTool\\IniPrefs");
	strcpy_safe((char *)0x006EC300, "Software\\iw\\CoDBORadiantModTool\\MRU");
	strcpy_safe((char *)0x006F0D08, "iw\\CoDBORadiantModTool");
	strcpy_safe((char *)0x006F7984, "CoDBORadiantModTool");
	strcpy_safe((char *)0x006ECA30, "You will need to restart CoDBORadiantModTool for the view changes to take place.");
	strcpy_safe((char *)0x006EC5CC, "CoDBORadiantModTool Project files( *.prj )|*.prj||");
	strcpy_safe((char *)0x00749640, "CoDBORadiantModTool Project (*.prj)");

	//
	// Debug INT3 to make sure specific functions are not called
	//
#if !WAW_PIMP
#define DO_NOT_USE(x) PatchMemory((x), (PBYTE)"\xCC", 1)
	DO_NOT_USE(0x0052EA20);// Material_ParseArgumentSource
	DO_NOT_USE(0x0052E2C0);// Material_ParseSamplerSource
	DO_NOT_USE(0x0052E6E0);// Material_ParseConstantSource
	DO_NOT_USE(0x0052F210);// Material_ParseShaderArguments
	DO_NOT_USE(0x0052E050);// Material_ParseIndexRange
	DO_NOT_USE(0x0052EFB0);// Material_AddShaderArgument
	DO_NOT_USE(0x0052ED40);// Material_SetShaderArguments
	DO_NOT_USE(0x0052EA70);// Material_DefaultArgumentSource
	DO_NOT_USE(0x0052E370);// Material_DefaultSamplerSourceFromTable
	DO_NOT_USE(0x0052E800);// Material_DefaultConstantSourceFromTable
	DO_NOT_USE(0x0052E430);// Material_DefaultSamplerSource
	DO_NOT_USE(0x0052E910);// Material_UnknownShaderworksConstantSource
	DO_NOT_USE(0x0052FDB0);// Material_GetStreamDestForSemantic
	DO_NOT_USE(0x0052DA70);// Material_NameForStreamDest
	DO_NOT_USE(0x0052E990);// Material_ElemCountForParamName
	DO_NOT_USE(0x0052D140);// Material_UsingTechnique
	DO_NOT_USE(0x0052F6B0);// Material_CopyTextToDXBuffer
	DO_NOT_USE(0x0052FE70);// Material_SetPassShaderArguments_DX
	DO_NOT_USE(0x00567450);// Image_LoadFromData
	DO_NOT_USE(0x0052B160);// Image_CreateCubeTexture_PC
	DO_NOT_USE(0x0052B040);// Image_Create3DTexture_PC
	DO_NOT_USE(0x0052AF20);// Image_Create2DTexture_PC
#undef DO_NOT_USE
#endif

	//
	// Increase the maximum number of files used by FS_ListFilteredFiles
	//
	int listSize = LISTSIZE_MAX - 1;
	int listAllocSize = (LISTSIZE_MAX) * 4 + 4;
	PatchMemory(0x004BD2E4, (PBYTE)&listAllocSize, 4);
	PatchMemory(0x004BD0E7, (PBYTE)&listSize, 4);
	PatchMemory(0x004D7072, (PBYTE)&listSize, 4);
	Detours::X86::DetourFunction((PBYTE)0x004D7066, (PBYTE)&mfh1_Sys_ListFiles);
	Detours::X86::DetourFunction((PBYTE)0x004D7093, (PBYTE)&mfh2_Sys_ListFiles);
	Detours::X86::DetourFunction((PBYTE)0x004D70DB, (PBYTE)&mfh3_Sys_ListFiles);

	//
	// (Deprecated): Leak pointfile compatibility fix
	//
	//PatchMemory(0x006F7378, (PBYTE)".pts", 4);

	//
	// Fix for misleading (incorrect) assertion message
	//
	const char* msg_assertion = "expected 'constant' or 'material', found '%s' instead\n";
	PatchMemory(0x0052E7F0, (PBYTE)&msg_assertion, 4);

	//
	// Fix for omni light previews being cut off at sqrt(radius)
	//
	R_SetLightProperties_o = (R_SetLightProperties_t)Detours::X86::DetourFunction((PBYTE)0x0056E4A0, (PBYTE)&R_SetLightProperties);

	//
	// Disable Spam
	//
#if RADIANT_DISABLE_SPAM_MSG_SCAN
	PatchMemory_WithNOP(0x004A7418, 5); //ScanDestructibleDef
	PatchMemory_WithNOP(0x004A74CD, 5); //StrCpy
	PatchMemory_WithNOP(0x004A71C2, 5); //ScanWeapon
	PatchMemory_WithNOP(0x0049B5F2, 5); //ScanFile
#endif
#if RADIANT_DISABLE_SPAM_MSG_IMAGE

	// ERROR: image '%s' is missing\n
	PatchMemory_WithNOP(0x0056762C, 5); // Com_Printf
	PatchMemory_WithNOP(0x00576C50, 5); // Com_Error
	
	// ERROR: failed to load image '%s'\n
	PatchMemory_WithNOP(0x0052B4D4, 5);
	PatchMemory_WithNOP(0x0052B61D, 5);
#endif
#if RADIANT_DISABLE_SPAM_MSG_MATERIAL
	PatchMemory_WithNOP(0x00472C28, 5); // WARNING: Could not find material '%s'
	PatchMemory_WithNOP(0x00519A5A, 5);
#endif

	//
	// Remap Statemap GE255 DepthTest -> LT128
	//
	Detours::X86::DetourFunction((PBYTE)0x0052D1B8, (PBYTE)&mfh_Material_ParseValueForState);

	//
	// Override BO1 codeConsts before pushing data to the shader
	//
	Detours::X86::DetourFunction((PBYTE)0x0053F625, (PBYTE)&mfh_R_SetPassPixelShaderStableArguments);

	//
	// Add support for BO1 KVs in the lighting preview
	// 
	Detours::X86::DetourFunction((PBYTE)0x00408367, (PBYTE)&mfh_Ent_BuildLightInfo);
	Detours::X86::DetourFunction((PBYTE)0x004A814E, (PBYTE)&mfh_Ent_Connect); // Generate the new default spotLight KVs when creating a spotLight

#if RADIANT_USE_AFX_OVERRIDES
	CWnd::OnCtlColor_o = (OnCtlColor_t)Detours::X86::DetourFunctionClass((PBYTE)0x0059B96E, &CWnd::OnCtlColor);
#endif

#if RADIANT_USE_SPLASH
	//
	// Generate Splash Screen on Launch (WinMain)
	//
	rtn_WinMain = Detours::X86::DetourFunction((PBYTE)0x00635355, (PBYTE)&mfh_WinMain, Detours::X86Option::USE_JUMP);

	//
	// Destroy Splash Screen Upon Entering MessageLoop
	//
	Com_LoadProject_o = (Com_LoadProject_t)Detours::X86::DetourFunction((PBYTE)0x0042E9C0, (PBYTE)&Com_LoadProject);

	//
	// Disable the initial SetWindowPlacement call for the corresponding windows
	// (Used to prevent the windows from showing upon placement, but prevents the placement from being restored from the registry keys)
	//
	//PatchMemory_WithNOP(0x0042F87E, 8); // Main Window (Resets the saved placement settings at every launch)
	PatchMemory_WithNOP(0x004B36DB, 8); // Entity Window
	PatchMemory_WithNOP(0x004018AC, 8); // Advanced Curve Dialog

	//
	// Override the initial SetWindowPlacement call for the corresponding windows
	// (Forces SW_HIDE for SetWindowPlacement, but doesn't always prevent certain windows from displaying)
	//
	void* ppfn = &pfn_SetWindowPlacement_Hidden;
	PatchMemory(0x0042F882, (PBYTE)&ppfn, 4); // Main Window
	//PatchMemory(0x004B36DF, (PBYTE)&ppfn, 4); // Entity Window (Doesn't work)
	//PatchMemory(0x004018B0, (PBYTE)&ppfn, 4); // Advanced Curve Dialog (Doesn''t work)
#endif

	//
	// Fix the grid block coordinates
	//
	PatchMemory(0x004827E9, (PBYTE)"\x84", 1);
	PatchMemory(0x004827D3, (PBYTE)"\x84", 1);

	//
	// Fix Crash when opening maps that don't have a valid skybox
	//	(Prevent attempting to use the default model as a skybox)
	//
	PatchCall(0x0040309F, (PBYTE)&R_RegisterSkyboxModel);

	//
	// Add Maps Loaded via Launcher (Args) to the MRU
	//
	PatchCall(0x0042DD2E, (PBYTE)&hk_HandleLaunchArgs);

	//
	// Live game update hooks
	//
	*(PBYTE *)&CCamWnd::ctor_o = Detours::X86::DetourFunction((PBYTE)0x00402B90, (PBYTE)&CCamWnd::ctor);

	*(PBYTE *)&Entity_Clone_o = Detours::X86::DetourFunction((PBYTE)0x0049E7C0, (PBYTE)&hk_Entity_Clone);
	*(PBYTE *)&Entity_Free_o = Detours::X86::DetourFunction((PBYTE)0x0049D3D0, (PBYTE)&hk_Entity_Free);

	*(PBYTE *)&Undo_Start_o = Detours::X86::DetourFunction((PBYTE)0x004769D0, (PBYTE)&hk_Undo_Start);
	*(PBYTE *)&Undo_End_o = Detours::X86::DetourFunction((PBYTE)0x00476E30, (PBYTE)&hk_Undo_End);

	*(PBYTE *)&SetEntityKeyValue_o = Detours::X86::DetourFunction((PBYTE)0x0049CE00, (PBYTE)&hk_SetEntityKeyValue);
	*(PBYTE *)&SetKeyValue_o = Detours::X86::DetourFunction((PBYTE)0x0049CD20, (PBYTE)&hk_SetKeyValue);

	*(PBYTE *)&Map_LoadFile_o = Detours::X86::DetourFunction((PBYTE)0x0049FF90, (PBYTE)&hk_Map_LoadFile);
	*(PBYTE *)&Map_SaveFile_o = Detours::X86::DetourFunction((PBYTE)0x004A07E0, (PBYTE)&hk_Map_SaveFile);

	*(PBYTE *)&MoveSelection_o = Detours::X86::DetourFunction((PBYTE)0x00498AE0, (PBYTE)&hk_MoveSelection);

	CreateThread(nullptr, 0, RemoteNet_Thread, nullptr, 0, nullptr);

	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		return RadiantMod_Init(); 
	}

	return TRUE;
}
