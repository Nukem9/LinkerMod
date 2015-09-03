#pragma once

#include "r_scene.h"

struct FxCameraUpdate
{
	float vieworg[3];
	float viewaxis[3][3];
	float tanHalfFovX;
	float tanHalfFovY;
	float zfar;
};

struct FxCmd
{
	void *system;
	int localClientNum;
	volatile int *spawnLock;
	bool useReflection;
	float reflectionHeight;
	unsigned int smodelCount;
	char *smodelVisData;
	float elemRemoveChance;
	char updateCameraType;
	void *visibleLights;
	int visibleLightCount;
	int updateEffectBegin;
	int updateEffectEnd;
};

typedef char __cdecl R_BeginFrame_t(void);
static R_BeginFrame_t* R_BeginFrame = (R_BeginFrame_t*)0x006D7AD0;

typedef void __cdecl R_EndFrame_t(void);
static R_EndFrame_t* R_EndFrame = (R_EndFrame_t*)0x006D7B90;


typedef void __cdecl R_RenderScene_t(refdef_s *refdef, int frameTime);
static R_RenderScene_t* R_RenderScene = (R_RenderScene_t*)0x006C8CD0;

typedef void __cdecl R_IssueRenderCommands_t(unsigned int type);
static R_IssueRenderCommands_t* R_IssueRenderCommands = (R_IssueRenderCommands_t*)0x006D5890;


typedef void __cdecl FX_BeginUpdate_t(int localClientNum);
static FX_BeginUpdate_t* FX_BeginUpdate = (FX_BeginUpdate_t*)0x0065AF40;

typedef void __cdecl FX_GetCameraUpdateFromRefdefAndZFar_t(FxCameraUpdate *cameraUpdate, refdef_s *refdef, float zfar);
static FX_GetCameraUpdateFromRefdefAndZFar_t* FX_GetCameraUpdateFromRefdefAndZFar = (FX_GetCameraUpdateFromRefdefAndZFar_t*)0x0062D3D0;

typedef void __cdecl FX_SetNextUpdateCamera_t(int localClientNum, FxCameraUpdate *cameraUpdate);
static FX_SetNextUpdateCamera_t* FX_SetNextUpdateCamera = (FX_SetNextUpdateCamera_t*)0x00653FA0;

typedef void __cdecl FX_FillUpdateCmd_t(int localClientNum, FxCmd *cmd);
static FX_FillUpdateCmd_t* FX_FillUpdateCmd = (FX_FillUpdateCmd_t*)0x005C8FB0;