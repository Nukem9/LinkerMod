#include "stdafx.h"
#include <algorithm>

struct MaterialMemory
{
	Material *material;
	int memory;
};

struct MaterialList
{
	unsigned int count;
	MaterialMemory sorted[4096*2]; // Ensure that there are enough entries for the increased limit
};

int R_GetMaterialMemory(Material* material)
{
	BYTE* world = *(BYTE**)0x0396A24C; // rgp.world

	int materialMemoryCount = *(int*)(world + 0x274);
	MaterialMemory* materialMemory = *(MaterialMemory**)(world + 0x278);
	
	if (world)
	{
		for (int i = 0; i < *(int*)(world + 0x274); i++)
		{
			if (material == materialMemory[i].material)
				return materialMemory[i].memory;
		}
	}

	return 0;
}

void __cdecl R_GetMaterialList(XAssetHeader header, MaterialList *materialList)
{
	int memory = R_GetMaterialMemory(header.material);
	if (memory)
	{
		ASSERT(materialList->count < _countof(materialList->sorted));
		MaterialMemory *materialMemory = &materialList->sorted[materialList->count];
		materialMemory->material = header.material;
		materialMemory->memory = memory;
		++materialList->count;
	}
}

void __cdecl R_MaterialList_f()
{
	unsigned int totalMemUsage = 0;
	
	Com_Printf(8, "-----------------------\n");
	
	MaterialList materialList;
	materialList.count = 0;
	DB_EnumXAssets(ASSET_TYPE_MATERIAL, (void(__cdecl *)(XAssetHeader, void *))R_GetMaterialList, &materialList, false);
	
	std::sort<MaterialMemory*, bool(__cdecl *)(MaterialMemory&, MaterialMemory&)>(materialList.sorted, &materialList.sorted[materialList.count],
		[](MaterialMemory& material0, MaterialMemory& material1) -> bool
		{
			return material0.memory < material1.memory;
		});

	Com_Printf(8, "geo KB   name\n");
	for (unsigned int mtlIndex = 0; mtlIndex < materialList.count; mtlIndex++)
	{
		MaterialMemory* materialMemory = &materialList.sorted[mtlIndex];
		Material* material = materialList.sorted[mtlIndex].material;
		ASSERT(material);

		totalMemUsage += materialMemory->memory;

		float value = (float)materialMemory->memory / 1024.0f;
		const char* fmt = (value >= 10.0f) ? "%6.0f" : "%6.1f";
		Com_Printf(8, fmt, value);
		Com_Printf(8, "   %s\n", material->info.name);
	}

	Com_Printf(8, "-----------------------\n");
	Com_Printf(8, "current total  %5.1f MB\n", (float)totalMemUsage / (1024.f * 1024.f));
	Com_Printf(8, "%i total geometry materials\n", materialList.count);
	Com_Printf(8, "Related commands: imagelist, gfx_world, cg_drawfps\n");
}

MaterialTechniqueSet *__cdecl Material_GetTechniqueSet(Material *material)
{
	ASSERT(material);
	ASSERT_MSG_VA(material->localTechniqueSet, "material '%s' missing techset. If you are building fastfile, check Launcher for error messages.", material->info.name);

	return material->localTechniqueSet;
}

MaterialTechnique *__cdecl Material_GetTechnique(Material *material, char techType)
{
	MaterialTechniqueSet* techSet = techSet = Material_GetTechniqueSet(material);
	ASSERT_MSG_VA(techSet, "material '%s' missing techset. %d tech %d", material->info.name, material->localTechniqueSet != 0, techType);

	return techSet->techniques[techType];
}
