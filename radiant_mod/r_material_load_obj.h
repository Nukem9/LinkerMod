#pragma once

bool Material_UsingTechnique(int techType);
bool Material_MatchToken(const char **text, const char *match);
bool Material_CopyTextToDXBuffer(void *cachedShader, unsigned int shaderLen, void **shader);
void *Material_LoadShader(const char *shaderName, const char *shaderVersion);
void *Material_RegisterTechnique(const char *name, int renderer);
bool Material_IgnoreTechnique(const char *name);
int Material_TechniqueTypeForName(const char *name);
void *__cdecl Material_LoadTechniqueSet(const char *name, int renderer);

void hk_Material_LoadShader();