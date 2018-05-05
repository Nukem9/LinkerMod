#pragma once

struct SpawnVar
{
	bool spawnVarsValid;
	int numSpawnVars;
	char *spawnVars[64][2];
	int numSpawnVarChars;
	char spawnVarChars[2048];
};
STATIC_ASSERT_OFFSET(SpawnVar, numSpawnVars, 0x4);
STATIC_ASSERT_OFFSET(SpawnVar, spawnVars, 0x8);

int G_ModelIndex(const char *name);
void G_OverrideModel(int modelIndex, const char *defaultModelName);