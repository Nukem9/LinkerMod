#include "stdafx.h"

bool Material_CopyTextToDXBuffer(void *cachedShader, unsigned int shaderLen, LPD3DXBUFFER *shader)
{
	HRESULT hr = D3DXCreateBuffer(shaderLen, shader);

	if (!SUCCEEDED(hr))
	{
		Com_PrintError(8, "ERROR: Material_CopyTextToDXBuffer: D3DXCreateBuffer(%d) failed: %s (0x%08x)\n", shaderLen, /*R_ErrorDescription(hr)*/"HRESULT", hr);

		free(cachedShader);
		return false;
	}

	memcpy((*shader)->GetBufferPointer(), cachedShader, shaderLen);
	return true;
}

FILE *Material_OpenShader_BlackOps(const char *shaderTarget, const char *shaderName)
{
	// Determine if this was a vertex shader or pixel shader
	const char *shaderMain;

	if (shaderTarget[0] == 'v' && shaderTarget[1] == 's')
		shaderMain = "vs";
	else
		shaderMain = "ps";

	// Load the shader directly from the name
	char shaderPath[MAX_PATH];
	sprintf_s(shaderPath, "%s\\raw\\shadercache_mods\\%s_%s_3_0_%s",
		fs_basepath->current.string,
		shaderTarget,
		shaderMain,
		shaderName);

	FILE *handle;
	if (fopen_s(&handle, shaderPath, "rb") != 0)
		return nullptr;
	
	return handle;
}

ID3DXBuffer *Material_CompileShader(const char *shaderName, int shaderType, const char *entryPoint, const char *target)
{
	FILE *shaderFile = Material_OpenShader_BlackOps(target, shaderName);

	if (!shaderFile)
		return nullptr;

	// Skip the first 4 bytes (shader ascii text length)
	fpos_t pos = 4;
	fsetpos(shaderFile, &pos);

	// Read the binary data size
	int shaderDataSize;
	if (fread(&shaderDataSize, 4, 1, shaderFile) < 1)
	{
		fclose(shaderFile);
		return nullptr;
	}

	void *shaderMemory = malloc(shaderDataSize);
	fread(shaderMemory, 1, shaderDataSize, shaderFile);

	ID3DXBuffer *shader = nullptr;
	if (!Material_CopyTextToDXBuffer(shaderMemory, shaderDataSize, &shader))
	{
		// Error message was already printed in above function
		ASSERT_MSG(false, "Shader upload failed");
	}

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

		push[ebp + 0xC]	// a4: target
		push[ebp + 0x8]	// a3: entryPoint
		push[ebp + 0x4]	// a2: shaderType
		push ecx		// a1: shaderName
		call Material_CompileShader
		add esp, 0x10

		pop ebp
		retn
	}
}
