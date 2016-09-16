#pragma once

bool Material_UsingTechnique_WAW(int techType);
const char *Material_NameForStreamDest_WAW(char dest);
bool Material_StreamDestForName_WAW(const char **text, const char *destName, char *dest);
bool Material_StreamSourceForName_WAW(const char **text, const char *sourceName, char *source);
char Material_GetStreamDestForSemantic_WAW(D3DXSEMANTIC *semantic);
int Material_TechniqueTypeForName_WAW(const char *name);