#pragma once

struct DiskGfxCubemap256
{
	char mip256[6][32768];

	struct
	{
		char mip128[8192];
		char mip64[2048];
		char mip32[512];
		char mip16[128];
		char mip8[32];
		char mip4[8];
		char mip2[8];
		char mip1[8];
	} mips[6];
};

struct ZoneCubemapFace256
{
	char mip256[32768];
	char mip128[8192];
	char mip64[2048];
	char mip32[512];
	char mip16[128];
	char mip8[32];
	char mip4[8];
	char mip2[8];
	char mip1[8];
};

BOOL IsReflectionMode();
BOOL IsInjectionMode();
BOOL ReflectionMod_Init();

BOOL ReflectionsWereUpdated();
BOOL InjectReflections();