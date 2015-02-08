#pragma once

FILE *Material_OpenShader_BlackOps(const char *shaderName, const char *shaderVersion);
FILE *Material_OpenShader_WAW(const char *shaderName, const char *shaderVersion);
void *Material_LoadShader(const char *shaderName, const char *shaderVersion);
bool Material_CopyTextToDXBuffer(void *cachedShader, unsigned int shaderLen, void **shader);

void hk_Material_LoadShader();