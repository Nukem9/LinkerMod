#include "stdafx.h"

SRCLINE(2819)
bool Material_UsingTechnique(int techType)
{
	ASSERT(techType <= 58);

	return ((BYTE *)0x0064B67C)[techType];
	//return g_useTechnique[techType];
}

SRCLINE(2825)
bool Material_MatchToken(const char **text, const char *match)
{
	return Com_MatchToken(text, match, 1) != 0;
}

SRCLINE(4720)
bool Material_CopyTextToDXBuffer(void *cachedShader, unsigned int shaderLen, void **shader)
{
	static DWORD dwCall = 0x0052F6B0;

	__asm
	{
		mov esi, shaderLen
		mov edi, shader
		mov ebx, cachedShader
		call [dwCall]
	}
}

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

	return fopen(shaderPath, "rb");
}

SRCLINE(4948)
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
		// Skip the first 4 bytes (zeros)
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

SRCLINE(8710)
void *Material_RegisterTechnique(const char *name, int renderer)
{
	static DWORD dwCall = 0x00530A40;

	__asm
	{
		mov esi, renderer
		mov eax, name
		call [dwCall]
	}
}

SRCLINE(8727)
bool Material_IgnoreTechnique(const char *name)
{
	const char *techniqueNames[1];
	techniqueNames[0] = "\"none\"";

	for (int techniqueIndex = 0; techniqueIndex < 1; techniqueIndex++)
	{
		if (!strcmp(name, techniqueNames[techniqueIndex]))
			return true;
	}

	return false;
}

SRCLINE(8763)
int Material_TechniqueTypeForName(const char *name)
{
	const char *techniqueNames[59];
	techniqueNames[0] = "\"depth prepass\"";
	techniqueNames[1] = "\"build floatz\"";
	techniqueNames[2] = "\"build shadowmap depth\"";
	techniqueNames[3] = "\"build shadowmap color\"";
	techniqueNames[4] = "\"unlit\"";
	techniqueNames[5] = "\"emissive\"";
	techniqueNames[6] = "\"emissive shadow\"";
	techniqueNames[7] = "\"emissive reflected\"";
	techniqueNames[8] = "\"lit\"";
	techniqueNames[9] = "\"lit fade\"";
	techniqueNames[10] = "\"lit sun\"";
	techniqueNames[11] = "\"lit sun fade\"";
	techniqueNames[12] = "\"lit sun shadow\"";
	techniqueNames[13] = "\"lit sun shadow fade\"";
	techniqueNames[14] = "\"lit spot\"";
	techniqueNames[15] = "\"lit spot fade\"";
	techniqueNames[16] = "\"lit spot shadow\"";
	techniqueNames[17] = "\"lit spot shadow fade\"";
	techniqueNames[18] = "\"lit omni\"";
	techniqueNames[19] = "\"lit omni fade\"";
	techniqueNames[20] = "\"lit omni shadow\"";
	techniqueNames[21] = "\"lit omni shadow fade\"";
	techniqueNames[22] = "\"lit charred\"";
	techniqueNames[23] = "\"lit fade charred\"";
	techniqueNames[24] = "\"lit sun charred\"";
	techniqueNames[25] = "\"lit sun fade charred\"";
	techniqueNames[26] = "\"lit sun shadow charred\"";
	techniqueNames[27] = "\"lit sun shadow fade charred\"";
	techniqueNames[28] = "\"lit spot charred\"";
	techniqueNames[29] = "\"lit spot fade charred\"";
	techniqueNames[30] = "\"lit spot shadow charred\"";
	techniqueNames[31] = "\"lit spot shadow fade charred\"";
	techniqueNames[32] = "\"lit omni charred\"";
	techniqueNames[33] = "\"lit omni fade charred\"";
	techniqueNames[34] = "\"lit omni shadow charred\"";
	techniqueNames[35] = "\"lit omni shadow fade charred\"";
	techniqueNames[36] = "\"lit instanced\"";
	techniqueNames[37] = "\"lit instanced sun\"";
	techniqueNames[38] = "\"lit instanced sun shadow\"";
	techniqueNames[39] = "\"lit instanced spot\"";
	techniqueNames[40] = "\"lit instanced spot shadow\"";
	techniqueNames[41] = "\"lit instanced omni\"";
	techniqueNames[42] = "\"lit instanced omni shadow\"";
	techniqueNames[43] = "\"light spot\"";
	techniqueNames[44] = "\"light omni\"";
	techniqueNames[45] = "\"light spot shadow\"";
	techniqueNames[46] = "\"light spot charred\"";
	techniqueNames[47] = "\"light omni charred\"";
	techniqueNames[48] = "\"light spot shadow charred\"";
	techniqueNames[49] = "\"fakelight normal\"";
	techniqueNames[50] = "\"fakelight view\"";
	techniqueNames[51] = "\"sunlight preview\"";
	techniqueNames[52] = "\"case texture\"";
	techniqueNames[53] = "\"solid wireframe\"";
	techniqueNames[54] = "\"shaded wireframe\"";
	techniqueNames[55] = "\"shadowcookie caster\"";
	techniqueNames[56] = "\"shadowcookie receiver\"";
	techniqueNames[57] = "\"debug bumpmap\"";
	techniqueNames[58] = "\"debug bumpmap instanced\"";

	for (int techniqueIndex = 0; techniqueIndex < 59; techniqueIndex++)
	{
		if (!strcmp(name, techniqueNames[techniqueIndex]))
			return techniqueIndex;
	}

	return 59;
}

SRCLINE(9023)
void *__cdecl Material_LoadTechniqueSet(const char *name, int renderer)
{
	char techType[130];

	//
	// Create a file path using normal techsets and read data
	//
	char filename[MAX_PATH];
	Com_sprintf(filename, MAX_PATH, "techsets/%s.techset", name);

	void *fileData;
	int fileSize = FS_ReadFile(filename, (void **)&fileData);

	if (fileSize < 0)
	{
		//
		// Try loading with PIMP enabled
		//
		Com_sprintf(filename, MAX_PATH, "pimp/techsets/%s.techset", name);
		fileSize = FS_ReadFile(filename, (void **)&fileData);

		if (fileSize < 0)
		{
			Com_PrintError(8, "^1ERROR: Couldn't open techniqueSet '%s'\n", filename);
			return nullptr;
		}
	}

	//
	// Allocate the techset structure
	//
	const char *textData	= (const char *)fileData;
	size_t nameSize			= strlen(name) + 1;
	char *techniqueSet		= (char *)Z_Malloc(nameSize + 248);

	*(char **)(techniqueSet + 0)	= techniqueSet + 248;
	*(BYTE *)(techniqueSet + 4)		= 0;
	*(char **)(techniqueSet + 8)	= techniqueSet;

	memcpy(techniqueSet + 248, name, nameSize);

	//
	// TODO: What does this function actually do?
	//
	((void(__cdecl *)())0x005525D0)();

	//
	// Begin the text parsing session
	//
	Com_BeginParseSession(filename);
	Com_SetScriptWarningPrefix("^1ERROR: ");
	Com_SetSpaceDelimited(0);
	Com_SetKeepStringQuotes(1);

	int techTypeCount	= 0;
	bool usingTechnique = false;
	while (1)
	{
		const char *token = Com_Parse(&textData);

		if (*token == '\0')
			break;

		if (*token == '"')
		{
			if (techTypeCount == 59)
			{
				Com_ScriptError("Too many labels in technique set\n");
				techniqueSet = 0;
				break;
			}

			if (!Material_IgnoreTechnique(token))
			{
				techType[techTypeCount] = Material_TechniqueTypeForName(token);

				if (techType[techTypeCount] == 59)
				{
					Com_ScriptError("Unknown technique type '%s'\n", token);
					techniqueSet = 0;
					break;
				}

				if (Material_UsingTechnique(techType[techTypeCount]))
					usingTechnique = true;

				++techTypeCount;
			}

			if (!Material_MatchToken(&textData, ":"))
			{
				techniqueSet = 0;
				break;
			}
		}
		else
		{
			if (usingTechnique)
			{
				if (!techTypeCount)
				{
					Com_ScriptError("Unknown technique type '%s'\n");
					techniqueSet = 0;
					break;
				}

				void *technique = Material_RegisterTechnique(token, renderer);
				if (!technique)
				{
					Com_ScriptError("Couldn't register technique '%s'\n");
					techniqueSet = 0;
					break;
				}

				for (int techTypeIndex = 0; techTypeIndex < techTypeCount; ++techTypeIndex)
					*(DWORD *)&techniqueSet[4 * techType[techTypeIndex] + 12] = (DWORD)technique;
			}

			techTypeCount	= 0;
			usingTechnique	= false;
			if (!Material_MatchToken(&textData, ";"))
			{
				techniqueSet = 0;
				break;
			}
		}
	}

	Com_EndParseSession();
	FS_FreeFile(fileData);
	return techniqueSet;
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