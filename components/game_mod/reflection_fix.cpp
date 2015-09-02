#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <shellapi.h>
#include <time.h>

char g_mapName[256];
bool g_reflectionsUpdated = false;
char g_ffDir[MAX_PATH] = "\0";

BOOL ReflectionsWereUpdated()
{
	return g_reflectionsUpdated;
}

BOOL IsReflectionMode()
{
	static bool hasChecked = false;
	static bool isReflectionMode = false;

	if(!hasChecked)
	{
		int argc = 0;
		LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);

		int flags = 0;
		for(int i = 0; i < argc-1; i++)
		{
			if(wcscmp(argv[i], L"+devmap") == 0 && flags ^ 1)
			{
				size_t convCount = 0;
				wcstombs_s(&convCount, g_mapName, argv[i+1],256);

				flags |= 1;
			}
			else if(wcscmp(argv[i], L"r_reflectionProbeGenerate") == 0 && wcscmp(argv[i+1], L"0") != 0 && flags ^ 2)
			{
				isReflectionMode = true;
				flags |= 2;
			}
			else if(flags == 3)
			{
				return isReflectionMode;
			}
		}
	}
	return isReflectionMode;
}

BOOL IsInjectionMode()
{
	static bool hasChecked = false;
	static bool isInjectionMode = false;

	if (!hasChecked)
	{
		int argc = 0;
		LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);

		for (int i = 0; i < argc - 2; i++)
		{
			if (wcscmp(argv[i], L"+set") == 0 && wcscmp(argv[i + 1], L"r_reflectionProbeInjectImages") == 0)
			{
				if (wcscmp(argv[i + 2], L"1") == 0)
				{
					isInjectionMode = true;
					return isInjectionMode;
				}
			}
		}
	}
	return isInjectionMode;
}

BOOL ReflectionMod_Init()
{
	if(!IsReflectionMode())
		return FALSE;

	//
	// Always force the cursor to be shown
	//
	//PatchMemory(0x00683C50, (PBYTE)"\xC3", 1); // sub_683C50
	//PatchMemory(0x00867927, (PBYTE)"\xEB", 1); // jz short loc_86793E
	//PatchMemory(0x006EB2CC, (PBYTE)"\xEB", 1); // jz short loc_6EB2DE

	//
	// Enable AutoExit Upon Completing Reflections
	//
	PatchMemory(0x006CF373, (PBYTE)"\x90\x90", 2);
	PatchMemory(0x006CF379, (PBYTE)"\x90\x90", 2);
	PatchMemory(0x006CF382, (PBYTE)"\x90\x90", 2);
	PatchMemory(0x006CF388, (PBYTE)"\x90\x90", 2);

	Detours::X86::DetourFunction((PBYTE)0x006CF150, (PBYTE)&hk_R_GenerateReflectionRawDataAll);
	Detours::X86::DetourFunction((PBYTE)0x006CEE30, (PBYTE)R_GenerateReflectionRawData);

	//
	// Check g_reflectionsUpdated for ReflectionExit (mov [g_reflectionsUpdate], 1)
	//
	BYTE buf[7] = {0xC6,0x05,0xFF,0xFF,0xFF,0xFF,0x01};
	bool* pTmp = &g_reflectionsUpdated;
	memcpy(buf+2,&pTmp,4);
	PatchMemory(0x006CF383, buf, 7);

	return TRUE;
}

char* formatTime(int seconds)
{
	static int index = 0;
	static char str[4][256]; //in case called by nested functions
	
	index = (index + 1) & 3;

	int hrs = seconds / 3600;
	seconds %= 3600;
	int mins = seconds / 60;
	seconds %= 60;

	if (hrs)
	{
		sprintf_s(str[index], "%d:%02d:%02d", hrs, mins, seconds);
	}
	else if (mins)
	{
		sprintf_s(str[index], "%d:%02d", mins, seconds);
	}
	else
	{
		sprintf_s(str[index], "%d seconds", seconds);
	}

	return str[index];
}

void __cdecl R_GenerateReflectionRawDataAll(DiskGfxReflectionProbe *probeRawData, int probeCount, bool *generateProbe)
{
	printf("----------------------------------------\n");
	printf("Compiling reflections...\n");

	time_t initTime;
	time_t cTime;
	time(&initTime);

	for (int probeIndex = 0; probeIndex < probeCount; probeIndex++)
	{
		if (generateProbe[probeIndex])
			R_GenerateReflectionRawData(&probeRawData[probeIndex]);

		time(&cTime);
		float percentComplete = (float)(probeIndex + 1) / (float)probeCount;
		float elapsedTime = (float)difftime(cTime, initTime);
		float remainingTime = elapsedTime / percentComplete - elapsedTime;

		printf("%.1f%% complete, %s done, %s remaining\r", percentComplete * 100.0f, formatTime((int)elapsedTime), formatTime((int)remainingTime));
	}

	printf("Finished in %s.\n", formatTime((int)difftime(cTime, initTime)));
	printf("----------------------------------------\n");
}

void __declspec(naked) hk_R_GenerateReflectionRawDataAll()
{
	__asm
	{
		push[esp + 8]	//generateProbe
		push[esp + 8]	//probeCount
		push eax		//probeRawData
		call R_GenerateReflectionRawDataAll
		add esp, 12
		retn
	}
}

void __cdecl R_GenerateReflectionRawData(DiskGfxReflectionProbe* probeRawData)
{
	if (!*g_ffDir)
	{
		sprintf_s(g_ffDir, "%s/%s/", Dvar_GetString("fs_basepath"), Dvar_GetString("fs_game"));
	}

	refdef_s refdef;
	char* ptr = *((char**)0x02FF5354);
	refdef_s* refsrc = (refdef_s*)(ptr + 0x8C100);
	memcpy(&refdef, refsrc, sizeof(refdef_s));

	refdef.vieworg[1] = probeRawData->origin[0];
	refdef.vieworg[2] = probeRawData->origin[1];
	refdef.yaw = probeRawData->origin[2];

	R_InitPrimaryLights(refdef.primaryLights[0].dir);

	for (int cubemapShot = 1; cubemapShot < 7; cubemapShot++)
	{
		R_BeginCubemapShot(256, 0);
		R_BeginFrame();

		GfxCmdArray* s_cmdList = *(GfxCmdArray**)0x03B370C0;
		GfxBackEndData* frontEndDataOut = *(GfxBackEndData**)0x03B3708C;
		frontEndDataOut->cmds = &s_cmdList->cmds[s_cmdList->usedTotal];
		frontEndDataOut->viewInfo[frontEndDataOut->viewInfoCount].cmds = NULL;

		R_ClearScene(0);
		FX_BeginUpdate(0);
		R_CalcCubeMapViewValues(&refdef, cubemapShot, 256);

		float* vec = (float*)0x3AC3060;
		vec[0] = refdef.vieworg[0];
		vec[1] = refdef.vieworg[1];
		vec[2] = refdef.vieworg[2];

		int* unk1 = (int*)0x3AC3058;		*unk1 = refdef.time;
		float* unk2 = (float*)0x3AC305C;	*unk2 = refdef.time * 0.001f;

		R_UpdateFrameFog(refdef.localClientNum);
		R_UpdateFrameSun();

		float zFar = R_GetFarPlaneDist();
		FxCameraUpdate fxUpdateCmd;
		memset(&fxUpdateCmd, 0, sizeof(FxCameraUpdate));
		FxCmd cmd;
		memset(&cmd, 0, sizeof(FxCmd));

		FX_GetCameraUpdateFromRefdefAndZFar(&fxUpdateCmd, &refdef, zFar);
		FX_SetNextUpdateCamera(0, &fxUpdateCmd);
		FX_FillUpdateCmd(0, &cmd);
		Sys_ResetUpdateSpotLightEffectEvent();
		Sys_AddWorkerCmdInternal((void*)0x00BA5420, &cmd, 0);
		Sys_ResetUpdateNonDependentEffectsEvent();
		Sys_AddWorkerCmdInternal((void*)0x00BA52E0, &cmd, 0);
		Sys_AddWorkerCmdInternal((void*)0x00BA5300, &cmd, 0);
		Sys_AddWorkerCmdInternal((void*)0x00BA5330, &cmd, 0);

		R_RenderScene(&refdef, 0);
		R_EndFrame();

		R_IssueRenderCommands(3);
		R_EndCubemapShot(cubemapShot);
	}

	R_CreateReflectionRawDataFromCubemapShot(probeRawData);
}

BOOL InjectReflections()
{
	return TRUE;
}