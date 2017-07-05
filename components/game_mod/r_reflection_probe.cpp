#include "stdafx.h"

// Declarations in patch_reflections.cpp
char* formatTime(int seconds);
extern bool	g_reflectionsUpdated;
extern char	g_ffDir[MAX_PATH];
extern int	g_probeCount;

VANILLA_VALUE(lockPvsViewParms, GfxViewParms, 0x0396F730);

VANILLA_VALUE(s_cmdList, GfxCmdArray*, 0x03B370C0);
VANILLA_VALUE(frontEndDataOut, GfxBackEndData*, 0x03B3708C);

void __declspec(naked) R_SetViewParmsForScene(refdef_s *refdef, GfxViewParms *viewParms)
{
	_asm
	{
		pushad
		mov eax, 0x006C7F80
		mov edi, viewParms
		mov esi, refdef
		call eax
		popad
		retn
	}
}

void R_CalcCubeMapViewValues(refdef_s *refdef, CubemapShot cubemapShot, int cubemapSize)
{
	((void(__cdecl *)(refdef_s *, CubemapShot, int))0x006CEC80)(refdef, cubemapShot, cubemapSize);
}

inline void R_BeginSharedCmdList(void)
{
	frontEndDataOut->cmds = &s_cmdList->cmds[s_cmdList->usedTotal];
}

inline void R_ClearClientCmdList2D(void)
{
	frontEndDataOut->viewInfo[frontEndDataOut->viewInfoCount].cmds = NULL;
}

inline void R_SetLodOrigin(refdef_s *refdef)
{
	if (r_lockPvs->modified)
	{
		Dvar_ClearModified(r_lockPvs);
		R_SetViewParmsForScene(refdef, &lockPvsViewParms);
	}

	float* vec = (float*)0x3AC3060;
	vec[0] = refdef->vieworg[0];
	vec[1] = refdef->vieworg[1];
	vec[2] = refdef->vieworg[2];

	int* unk1 = (int*)0x3AC3058;		*unk1 = refdef->time;
	float* unk2 = (float*)0x3AC305C;	*unk2 = refdef->time * 0.001f;

	R_UpdateFrameFog(refdef->localClientNum);
	R_UpdateFrameSun();
}

inline void R_UpdateSpotLightEffect(FxCmd *cmd)
{
	Sys_ResetUpdateSpotLightEffectEvent();
	Sys_AddWorkerCmdInternal((void*)0x00BA5420, cmd, 0);
}

inline void R_UpdateNonDependentEffects(FxCmd *cmd)
{
	Sys_ResetUpdateNonDependentEffectsEvent();
	Sys_AddWorkerCmdInternal((void*)0x00BA52E0, cmd, 0);
}

inline void R_UpdateRemainingEffects(FxCmd *cmd)
{
	Sys_AddWorkerCmdInternal((void*)0x00BA5300, cmd, 0);
	Sys_AddWorkerCmdInternal((void*)0x00BA5330, cmd, 0);
}

void R_GenerateReflectionRawData(DiskGfxReflectionProbe* probeRawData)
{
	if (!*g_ffDir)
		sprintf_s(g_ffDir, "%s/%s/", Dvar_GetString("fs_basepath"), Dvar_GetString("fs_game"));

	refdef_s refdef;
	char* ptr = *((char**)0x02FF5354);
	refdef_s* refsrc = (refdef_s*)(ptr + 0x8C100);
	memcpy(&refdef, refsrc, sizeof(refdef_s));

	refdef.vieworg[1] = probeRawData->origin[0];
	refdef.vieworg[2] = probeRawData->origin[1];
	refdef.yaw = probeRawData->origin[2];

	R_InitPrimaryLights(refdef.primaryLights[0].dir);

	FxCmd cmd;
	FxCameraUpdate fxCam;

	for (int cubemapShot = 1; cubemapShot < 7; cubemapShot++)
	{
		R_BeginCubemapShot(256, 0);
		R_BeginFrame();
		R_BeginSharedCmdList();
		R_ClearClientCmdList2D();

		R_ClearScene(0);

		FX_BeginUpdate(0);
		R_CalcCubeMapViewValues(&refdef, cubemapShot, 256);

		R_SetLodOrigin(&refdef);

		R_UpdateFrameFog(refdef.localClientNum);
		R_UpdateFrameSun();

		float zFar = R_GetFarPlaneDist();

		// Not entirely sure if this is needed or not...
		//memset(&fxCam, 0, sizeof(FxCameraUpdate));
		//memset(&cmd, 0, sizeof(FxCmd));

		FX_GetCameraUpdateFromRefdefAndZFar(&fxCam, &refdef, zFar);
		FX_SetNextUpdateCamera(0, &fxCam);
		FX_FillUpdateCmd(0, &cmd);

		R_UpdateSpotLightEffect(&cmd);
		R_UpdateNonDependentEffects(&cmd);
		R_UpdateRemainingEffects(&cmd);

		R_RenderScene(&refdef, 0);
		R_EndFrame();

		R_IssueRenderCommands(3);
		R_EndCubemapShot(cubemapShot);
	}

	R_CreateReflectionRawDataFromCubemapShot(probeRawData);
}

void R_GenerateReflectionRawDataAll(DiskGfxReflectionProbe *probeRawData, int probeCount, bool *generateProbe)
{
	Com_ToolPrintf(0, "----------------------------------------\n");
	Com_ToolPrintf(0, "Generating reflections...\n");
	fflush(stdout);

	g_probeCount = probeCount;

	time_t initTime;
	time_t currTime;
	time(&initTime);

	for (int probeIndex = 0; probeIndex < probeCount; probeIndex++)
	{
		if (generateProbe[probeIndex])
			R_GenerateReflectionRawData(&probeRawData[probeIndex]);

		time(&currTime);
		float percentComplete = (float)(probeIndex + 1) / (float)probeCount;
		float elapsedTime = (float)difftime(currTime, initTime);
		float remainingTime = elapsedTime / percentComplete - elapsedTime;

		Com_ToolPrintf(0, "%.1f%% (%d of %d) complete, %s done, %s remaining\n",
			percentComplete * 100.0f,
			probeIndex + 1,
			probeCount,
			formatTime((int)elapsedTime),
			formatTime((int)remainingTime));
		fflush(stdout);
	}

	Com_ToolPrintf(0, "Finished in %s.\n", formatTime((int)difftime(currTime, initTime)));
	printf("----------------------------------------\n");
}

bool R_CopyReflectionsFromLumpData(DiskGfxReflectionProbe *probeRawData, DiskGfxReflectionProbe *probeRawLumpData, const int lumpProbeCount)
{
	if (lumpProbeCount <= 0)
		return false;

	// TODO: Figure out what the LAHF instructions are
	return false;
}

void R_GenerateReflections(const char *mapname, GfxReflectionProbe *probes, const unsigned int probeCount)
{
	//return;

	ASSERT(r_reflectionProbeGenerate);
	ASSERT(probeCount < MAX_MAP_REFLECTION_PROBES);

	// Skip this function if the user didn't explicitly set dvar
	if (!r_reflectionProbeGenerate->current.enabled)
		return;

	// Version checking
	Com_LoadBsp(mapname);
	unsigned int version = Com_GetBspVersion();

	if (version != 45)
	{
		Com_Error(
			ERR_DROP,
			"You can only generate reflections for BSP version %i, but the BSP is version %i. You need to recompile the map.",
			45,
			version);
		return;
	}

	unsigned int lumpSize							= 0;
	unsigned int lumpProbeCount						= 0;
	DiskGfxReflectionProbe *probeRawLumpData		= nullptr;
	DiskGfxReflectionProbe *probeRawGeneratedData	= nullptr;

	probeRawLumpData = (DiskGfxReflectionProbe *)Com_GetBspLump(LUMP_REFLECTION_PROBES, sizeof(DiskGfxReflectionProbe), &lumpProbeCount);

	ASSERT(probeCount == lumpProbeCount);

	// Generate images for each probe in the BSP
	if (probeCount)
	{
		bool generateProbe[MAX_MAP_REFLECTION_PROBES];
		memset(generateProbe, 0, sizeof(generateProbe));

		lumpSize				= sizeof(DiskGfxReflectionProbe) * probeCount;
		probeRawGeneratedData	= (DiskGfxReflectionProbe *)Z_Malloc(lumpSize, "R_GenerateReflections", 0);

		for (unsigned int probeIndex = 0; probeIndex < lumpProbeCount; ++probeIndex)
		{
			strcpy_s(probeRawGeneratedData[probeIndex].colorCorrectionFilename, probeRawLumpData[probeIndex].colorCorrectionFilename);

			probeRawGeneratedData[probeIndex].origin[0] = probes[probeIndex].origin[0];
			probeRawGeneratedData[probeIndex].origin[1] = probes[probeIndex].origin[1];
			probeRawGeneratedData[probeIndex].origin[2] = probes[probeIndex].origin[2];

			if (r_reflectionProbeRegenerateAll->current.enabled)
			{
				// Generate every single probe, ignoring anything in the BSP
				generateProbe[probeIndex] = true;
			}
			else
			{
				// See if we can copy old reflection data from the map
				generateProbe[probeIndex] = !R_CopyReflectionsFromLumpData(&probeRawGeneratedData[probeIndex], probeRawLumpData, probeCount);
			}
		}

		R_GenerateReflectionRawDataAll(probeRawGeneratedData, lumpProbeCount, generateProbe);

		Com_SaveLump(LUMP_REFLECTION_PROBES, probeRawGeneratedData, lumpSize, COM_SAVE_LUMP_AND_CLOSE);
		Com_ToolPrintf(0, "Reflections saved with lump size %u\n", lumpSize);

		R_GenerateReflectionImages(probes, probeRawGeneratedData, probeCount, 0);
		Z_Free(probeRawGeneratedData, 0);
	}
	else
	{
		Com_SaveLump(LUMP_REFLECTION_PROBES, nullptr, 0, COM_SAVE_LUMP_AND_CLOSE);
		Com_ToolPrintf(0, "Reflections saved with empty lump data\n");
	}

	// Custom tracker for injected reflections
	g_reflectionsUpdated = true;

	// Terminate the game if there's nothing left to do
	if (R_ReflectionProbeGenerateExitWhenDone())
	{
		Sys_NormalExit();
		exit(0);
	}

	// Allow viewing the map after images are generated
	Dvar_SetBool(r_reflectionProbeGenerate, false);
}

bool R_ReflectionProbeGenerateExitWhenDone()
{
	return r_reflectionProbeGenerate && r_reflectionProbeGenerate->current.enabled && 
		r_reflectionProbeGenerateExit && r_reflectionProbeGenerateExit->current.enabled;
}

void R_GenerateReflectionImages(GfxReflectionProbe *probes, DiskGfxReflectionProbe *probeRawData, const int probeCount, const int probeBaseIndex)
{
	((void(__cdecl *)(GfxReflectionProbe *, DiskGfxReflectionProbe *, const int, const int))0x006CF810)(probes, probeRawData, probeCount, probeBaseIndex);
}

void __declspec(naked) hk_R_GenerateReflections()
{
	__asm
	{
		push[esp + 8]	// probeCount
		push[esp + 8]	// probes
		push ecx		// mapname
		call R_GenerateReflections
		add esp, 0xC
		retn
	}
}