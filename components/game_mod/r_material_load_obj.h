#pragma once

#include "../shared/minidx9/Include/d3dx9.h"
#pragma comment(lib, "../shared/minidx9/Lib/x86/d3dx9.lib")

bool Material_CopyTextToDXBuffer(void *cachedShader, unsigned int shaderLen, LPD3DXBUFFER *shader);

FILE *Material_OpenShader_BlackOps(const char *shaderTarget, const char *shaderName);

ID3DXBuffer *Material_CompileShader(const char *shaderName, int shaderType, const char *entryPoint, const char *target);
void hk_Material_CompileShader();
