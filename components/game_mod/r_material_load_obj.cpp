#include "stdafx.h"

#include <stdio.h>
#include "../shared/minidx9/Include/d3dx9.h"
#pragma comment(lib, "../shared/minidx9/Lib/x86/d3dx9.lib")

bool Material_CopyTextToDXBuffer(void *cachedShader, unsigned int shaderLen, LPD3DXBUFFER *shader)
{
	HRESULT hr = D3DXCreateBuffer(shaderLen, shader);

	if (!SUCCEEDED(hr))
	{
		// Com_PrintError(8, "ERROR: Material_CopyTextToDXBuffer: D3DXCreateBuffer(%d) failed: %s (0x%08x)\n", shaderLen, R_ErrorDescription(hr), hr);

		free(cachedShader);
		return false;
	}

	memcpy((*shader)->GetBufferPointer(), cachedShader, shaderLen);
	return true;
}

FILE *Material_OpenShader_BlackOps(const char *shaderTarget, const char *shaderName)
{
	//
	// Determine if this was a vertex shader or pixel shader
	//
	const char *shaderMain;

	if (shaderTarget[0] == 'v' && shaderTarget[1] == 's')
		shaderMain = "vs";
	else
		shaderMain = "ps";

	//
	// Load the shader directly from the name
	//
	char shaderPath[MAX_PATH];
	sprintf_s(shaderPath, "%s\\raw\\shadercache_mods\\%s_%s_3_0_%s",
		*(DWORD *)(*(DWORD *)0x25FBF04 + 0x18),
		shaderTarget,
		shaderMain,
		shaderName);

	FILE* h = NULL;
	if (fopen_s(&h, shaderPath, "rb") != 0)
	{
		return NULL;
	}
	
	return h;
}

ID3DXBuffer *Material_CompileShader(const char *shaderName, int shaderType, const char *entryPoint, const char *target)
{
	int shaderDataSize = 0;
	ID3DXBuffer *shader = nullptr;
	FILE *shaderFile = Material_OpenShader_BlackOps(target, shaderName);

	if (!shaderFile)
		return nullptr;

	// Skip the first 4 bytes (zeros)
	fpos_t pos = 4;
	fsetpos(shaderFile, &pos);

	// Read the real data size
	if (fread(&shaderDataSize, 4, 1, shaderFile) < 1)
	{
		fclose(shaderFile);
		return nullptr;
	}

	void *shaderMemory = malloc(shaderDataSize);
	fread(shaderMemory, 1, shaderDataSize, shaderFile);

	if (!Material_CopyTextToDXBuffer(shaderMemory, shaderDataSize, &shader))
		ASSERT_MSG(false, "SHADER UPLOAD FAILED\n");

	fclose(shaderFile);
	free(shaderMemory);
	return shader;
}

void __declspec(naked) hk_Material_CompileShader()
{
	__asm
	{
		push ebp
		mov ebp, esp

		push[ebp + 0xC]	// target
		push[ebp + 0x8]	// entryPoint
		push[ebp + 0x4]	// shaderType
		push ecx		// shaderName
		call Material_CompileShader
		add esp, 0x10

		pop ebp
		retn
	}
}
