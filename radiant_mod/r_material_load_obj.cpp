#include "stdafx.h"

FILE *Material_OpenShader_BlackOps(const char *shaderName, const char *shaderVersion)
{
	//
	// Determine if this was a vertex shader or pixel shader
	//
	const char *shaderMain;

	if (shaderVersion[0] == 'v' && shaderVersion[1] == 's')
		shaderMain = "vs_main";
	else
		shaderMain = "ps_main";

	//
	// Load the shader directly from the name
	//
	char shaderPath[MAX_PATH];
	Com_sprintf(shaderPath, MAX_PATH, "%s\\raw\\shadercache_mods\\%s_%s_%s",
		*(DWORD *)(*(DWORD *)0x258E9F0 + 12),
		shaderMain,
		shaderVersion,
		shaderName);

	printf("BO SHADER: %s\n", shaderPath);
	return fopen(shaderPath, "rb");
}

FILE *Material_OpenShader_WAW(const char *shaderName, const char *shaderVersion)
{
	//
	// Create a unique shader string to hash
	//
	char shaderString[MAX_PATH];
	Com_sprintf(shaderString, MAX_PATH, "%s_%s", shaderVersion, shaderName);

	//
	// Determine the path to load the shader from
	//
	char shaderPath[MAX_PATH];
	Com_sprintf(shaderPath, MAX_PATH, "%s\\raw\\shader_bin\\%s_%8.8x",
		*(DWORD *)(*(DWORD *)0x258E9F0 + 12),
		shaderVersion,
		R_HashAssetName(shaderString));

	printf("WAW SHADER: %s %s\n", shaderString, shaderPath);
	return fopen(shaderPath, "rb");
}

void *Material_LoadShader(const char *shaderName, const char *shaderVersion)
{
	//
	// Try loading the black ops version first
	//
	int shaderDataSize	= 0;
	FILE *shaderFile	= Material_OpenShader_BlackOps(shaderName, shaderVersion);
	
	if (shaderFile)
	{
		//
		// Skip the first 4 bytes
		//
		fpos_t pos = 4;
		fsetpos(shaderFile, &pos);

		//
		// Read the real data size
		//
		if (fread(&shaderDataSize, 4, 1, shaderFile) < 1)
		{
			fclose(shaderFile);
			return 0;
		}
	}
	else
	{
		//
		// Load the WAW version if it wasn't found
		//
		if (!shaderFile)
			shaderFile = Material_OpenShader_WAW(shaderName, shaderVersion);

		if (!shaderFile)
			return 0;

		if (fread(&shaderDataSize, 4, 1, shaderFile) < 1)
		{
			fclose(shaderFile);
			return 0;
		}
	}

	void *shaderMemory = Z_Malloc(shaderDataSize);
	fread(shaderMemory, 1, shaderDataSize, shaderFile);

	void *shader = nullptr;
	if (!Material_CopyTextToDXBuffer(shaderMemory, shaderDataSize, &shader))
		printf("SHADER CREATION FAILED\n");

	fclose(shaderFile);
	Z_Free(shaderMemory);
	return shader;
}

bool Material_CopyTextToDXBuffer(void *cachedShader, unsigned int shaderLen, void **shader)
{
	static DWORD dwCall = 0x52F6B0;

	__asm
	{
		mov esi, shaderLen
		mov edi, shader
		mov ebx, cachedShader
		call [dwCall]
	}
}

void __declspec(naked) hk_Material_LoadShader()
{
	__asm
	{
		push ebp
		mov ebp, esp

		push ecx
		push [ebp + 0x8]
		call Material_LoadShader
		add esp, 0x8

		pop ebp
		retn
	}
}