#include "stdafx.h"
#include "r_material_load_obj.h"

enum
{
	SAMPLER_FILTER_SHIFT = 0x0,
	SAMPLER_FILTER_NEAREST = 0x1,
	SAMPLER_FILTER_LINEAR = 0x2,
	SAMPLER_FILTER_ANISO2X = 0x3,
	SAMPLER_FILTER_ANISO4X = 0x4,
	SAMPLER_FILTER_MASK = 0x7,
	SAMPLER_MIPMAP_SHIFT = 0x3,
	SAMPLER_MIPMAP_DISABLED = 0x0,
	SAMPLER_MIPMAP_NEAREST = 0x8,
	SAMPLER_MIPMAP_LINEAR = 0x10,
	SAMPLER_MIPMAP_COUNT = 0x3,
	SAMPLER_MIPMAP_MASK = 0x18,
	SAMPLER_CLAMP_U_SHIFT = 0x5,
	SAMPLER_CLAMP_V_SHIFT = 0x6,
	SAMPLER_CLAMP_W_SHIFT = 0x7,
	SAMPLER_CLAMP_U = 0x20,
	SAMPLER_CLAMP_V = 0x40,
	SAMPLER_CLAMP_W = 0x80,
	SAMPLER_CLAMP_MASK = 0xE0,
	SAMPLER_ANISO_SHIFT = 0x8,
	SAMPLER_ANISO_1X = 0x0,
	SAMPLER_ANISO_2X = 0x100,
	SAMPLER_ANISO_4X = 0x200,
	SAMPLER_ANISO_6X = 0x300,
	SAMPLER_ANISO_8X = 0x400,
	SAMPLER_ANISO_10X = 0x500,
	SAMPLER_ANISO_12X = 0x600,
	SAMPLER_ANISO_16X = 0x700,
	SAMPLER_ANISO_MASK = 0x700,
	SAMPLER_CONVOLUTION = 0x20000,
	SAMPLER_GAMMA = 0x40000,
	SAMPLER_UNNORMALIZED_UV = 0x80000,
	SAMPLER_DIRECT_FILTER_UNNORMALIZED = 0x80000,
};

struct MaterialTypeInfo
{
	const char *prefix;
	const char *techniqueSetPrefix;
	unsigned int prefixLen;
};

MaterialTypeInfo* g_materialTypeInfo = (MaterialTypeInfo*)0x005717E0;

water_t * Material_RegisterWaterImage(struct MaterialWaterDef *water)
{
	static DWORD dwCall = 0x00484B40;

	water_t* result = NULL;

	_asm
	{
		pushad
		mov esi, water
		call dwCall
		mov result, eax
		popad
	}

	return result;
}

bool __cdecl Material_FinishLoadingInstance(MaterialRaw *mtlRaw, const char *techniqueSetVertDeclPrefix, MaterialTechniqueSet **techniqueSet, unsigned int materialType)
{
	static DWORD dwCall = 0x00484C70;

	bool result=false;

	_asm
	{
		pushad
		push techniqueSetVertDeclPrefix
		mov edx, materialType
		mov esi, mtlRaw
		mov ecx, techniqueSet
		call dwCall
		mov result, al
		add esp, 4
		popad
	}
	
	return result;
}

unsigned int __cdecl Material_GetTechniqueSetDrawRegion(MaterialTechniqueSet *techniqueSet)
{
	int cameraRegion;

	ASSERT(techniqueSet);

	if (techniqueSet->techniques[10])
		cameraRegion = 0;
	else if (techniqueSet->techniques[5])
		cameraRegion = 2;
	else
		cameraRegion = 3;
	return cameraRegion;
}

void Material_SetMaterialDrawRegion(Material *material)
{
	unsigned int cameraRegion = Material_GetTechniqueSetDrawRegion(material->localTechniqueSet);
	if (!cameraRegion && material->info.sortKey >= 24)
		cameraRegion = 1;
	material->cameraRegion = (char)cameraRegion;
}

Material *__cdecl Material_LoadRaw(MaterialRaw *mtlRaw, unsigned int materialType)
{
	MaterialTechniqueSet *techniqueSet;
	if (Material_FinishLoadingInstance(
		mtlRaw,
		g_materialTypeInfo[materialType].techniqueSetPrefix,
		&techniqueSet,
		materialType))
	{
		char* name = (char *)mtlRaw + mtlRaw->info.nameOffset;
		unsigned int prefixLen = g_materialTypeInfo[materialType].prefixLen;
		unsigned int nameLen = strlen(name);
		Material* materialMem = (Material *)Material_Alloc(prefixLen + nameLen + 1 + 192);
		Material* material = materialMem;
		char* strDest = (char *)&materialMem[1];
		memcpy((char *)&materialMem[1], (char *)g_materialTypeInfo[materialType].prefix, prefixLen);
		memcpy(&strDest[prefixLen], name, nameLen + 1);
		material->info.name = strDest;

		//ASSERT(!material->info.drawSurf.fields.materialSortedIndex);
		//ASSERT(!((unsigned __int64)material->info.drawSurf.fields._bf0 >> 31) & 0xFFF);

		material->info.gameFlags = mtlRaw->info.gameFlags;
		unsigned int surfIndex = (unsigned __int8)((mtlRaw->info.surfaceFlags & 0x3F00000) >> 20);

		if ((unsigned __int8)((mtlRaw->info.surfaceFlags & 0x3F00000) >> 20))
		{
			material->info.surfaceTypeBits = 1 << (surfIndex - 1);
			material->info.layeredSurfaceTypes = surfIndex;
		}
		else
		{
			material->info.surfaceTypeBits = 0;
			material->info.layeredSurfaceTypes = 0;
		}

		material->info.layeredSurfaceTypes |= 0x20000000u;

		if (materialType != 3 && materialType != 4)
			material->info.gameFlags &= 0xFFFFFFFD;   // disable bit 2

		material->info.sortKey = mtlRaw->info.sortKey;
		material->info.textureAtlasRowCount = mtlRaw->info.textureAtlasRowCount;
		material->info.textureAtlasColumnCount = mtlRaw->info.textureAtlasColumnCount;
		material->textureCount = (BYTE)mtlRaw->textureCount;
		material->constantCount = (BYTE)mtlRaw->constantCount;
		material->localTechniqueSet = techniqueSet;

		if (mtlRaw->textureCount)
		{
			material->textureTable = (MaterialTextureDef *)Material_Alloc(16 * mtlRaw->textureCount);
			MaterialTextureDefRaw* textureTableRaw = (MaterialTextureDefRaw *)((char *)mtlRaw + mtlRaw->textureTableOffset);
			int texIndex = 0;
			for (unsigned int texIndexIn = 0; texIndexIn < mtlRaw->textureCount; ++texIndexIn)
			{
				char* tableEntryName = (char *)mtlRaw + textureTableRaw[texIndexIn].nameOffset;

				material->textureTable[texIndex].nameHash = R_HashString(tableEntryName);
				material->textureTable[texIndex].nameStart = *tableEntryName;
				material->textureTable[texIndex].nameEnd = tableEntryName[strlen(tableEntryName) - 1];
				material->textureTable[texIndex].samplerState = textureTableRaw[texIndexIn].samplerState;
				
				ASSERT(material->textureTable[texIndex].samplerState & SAMPLER_FILTER_MASK);

				material->textureTable[texIndex].semantic = textureTableRaw[texIndexIn].semantic;
				material->textureTable[texIndex].isMatureContent = 0;

				if (material->textureTable[texIndex].semantic == 11)
				{
					material->textureTable[texIndex++].u.image = (GfxImage *)Material_RegisterWaterImage((MaterialWaterDef *)((char *)mtlRaw + textureTableRaw[texIndexIn].u.imageNameOffset));
				}
				else if (material->textureTable[texIndex].semantic != 28)
				{
					char* imageName = (char *)mtlRaw + textureTableRaw[texIndexIn].u.imageNameOffset;
					material->textureTable[texIndex].u.image = Image_Register(imageName, textureTableRaw[texIndexIn].semantic);
					material->textureTable[texIndex++].isMatureContent = MaterialTexture_IsMatureContent( material->textureTable[texIndexIn].semantic, imageName);
				}
			}
			material->textureCount = texIndex;
		}

		if (mtlRaw->constantCount)
		{
			material->localConstantTable = (MaterialConstantDef *)Material_Alloc(32 * mtlRaw->constantCount);
			MaterialConstantDefRaw* constantTableRaw = (MaterialConstantDefRaw *)((char *)mtlRaw + mtlRaw->constantTableOffset);
			
			for (unsigned int constIndex = 0; constIndex < mtlRaw->constantCount; ++constIndex)
			{
				char* constName = (char *)mtlRaw + constantTableRaw[constIndex].nameOffset;
				material->localConstantTable[constIndex].nameHash = R_HashString(constName);

				strcpy_s(material->localConstantTable[constIndex].name, 12, constName);

				float* _literal = material->localConstantTable[constIndex]._literal;
				float* literal_raw = constantTableRaw[constIndex].literal;
				_literal[0] = literal_raw[0];
				_literal[1] = literal_raw[1];
				_literal[2] = literal_raw[2];
				_literal[3] = literal_raw[3];
			}
		}

		Material_BuildStateBitsTable(material, mtlRaw->info.toolFlags, mtlRaw->refStateBits);
		Material_SetMaterialDrawRegion(material);

		if (Material_Validate(material))
		{
			ASSERT(material->techniqueSet);
			ASSERT(!(material->info.gameFlags & MTL_GAMEFLAG_CASTS_SHADOW));

			if (!(mtlRaw->info.surfaceFlags & 0x40000)
				&& Material_GetTechnique(material, 2) != 0
				&& !(material->stateFlags & 4))
			{
				material->info.gameFlags |= 0x40u;
			}
			if (mtlRaw->info.surfaceFlags & 0x80000 && Material_GetTechnique(material, 2) != 0 && !(material->stateFlags & 4))
				material->info.gameFlags |= 1u;
			if (mtlRaw->info.surfaceFlags & 0x100)
				material->info.gameFlags |= 0x80u;
			if (mtlRaw->info.surfaceFlags & 0x200)
				material->info.gameFlags |= 0x100u;
			if (mtlRaw->info.surfaceFlags & 0x800)
				material->info.gameFlags |= 0x200u;
			
			return material;
		}
		
		return NULL;
	}

	return NULL;
}