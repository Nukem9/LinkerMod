#include "AssetHandler.h"
#include <unordered_map>
#include "ForeignPointer.h"

//
// These was an issue when using const char* keys
//
#include <string>
static std::unordered_map<std::string, ForeignPointer<MaterialTechniqueSet>> techsets;
static std::unordered_map<std::string, ForeignPointer<MaterialTechniqueSet>> overrideTechsets;
static std::unordered_map<std::string, ForeignPointer<MaterialTechnique>> techniques;
static std::unordered_map<std::string, ForeignPointer<MaterialTechnique>> overrideTechniques;
static std::unordered_map<std::string, ForeignPointer<MaterialVertexShader>> vShaders;
static std::unordered_map<std::string, ForeignPointer<MaterialVertexShader>> overrideVShaders;
static std::unordered_map<std::string, ForeignPointer<MaterialPixelShader>> pShaders;
static std::unordered_map<std::string, ForeignPointer<MaterialPixelShader>> overridePShaders;

int techsetCount = 0;
int overrideTechsetCount = 0;
int techniqueCount = 0;
int overrideTechniqueCount = 0;
int vShaderCount = 0;
int overrideVShaderCount = 0;
int pShaderCount = 0;
int overridePShaderCount = 0;

int LocateShaders(ForeignPointer<MaterialTechnique> tech, bool forceOverride = false)
{
	SIZE_T numofbytesread = 0;
	char buf[256];

	if(!tech->passCount)
	{
		return 1;
	}

	ForeignPointer<MaterialVertexShader> vertexShader(tech->passArray[0].vertexShader);
	ReadProcessMemory(g_hProcess,vertexShader->name,buf,256,&numofbytesread);
	std::string name = buf;
	if(vShaders.count(name) == 0 && !forceOverride)
	{
		vShaderCount++;
		vShaders[name] = vertexShader;
	}
	else if(overrideVShaders.count(name) == 0 && vShaders[name].pAddress != vertexShader.pAddress)
	{
		overrideVShaderCount++;
		overrideVShaders[name] = vertexShader;
	}

	ForeignPointer<MaterialPixelShader> pixelShader(tech->passArray[0].pixelShader);
	ReadProcessMemory(g_hProcess,pixelShader->name,buf,256,&numofbytesread);
	name = buf;
	if(pShaders.count(name) == 0 && !forceOverride)
	{
		pShaderCount++;
		pShaders[name] = pixelShader;
	}
	else if(overridePShaders.count(name) == 0 && pShaders[name].pAddress != pixelShader.pAddress)
	{
		overridePShaderCount++;
		overridePShaders[name] = pixelShader;
	}

	return 0;
};

int LocateTechnique(ForeignPointer<MaterialTechniqueSet> techset)
{
	SIZE_T numofbytesread = 0;
	char str[256];

	for(int i = 0; i < 130; i++)
	{
		ForeignPointer<MaterialTechnique> tech(techset->techniques[i]);

		if(tech.pAddress)
		{
			ReadProcessMemory(g_hProcess,tech->name,str,256,&numofbytesread);
			std::string key = str;

			if(techniques.count(key) == 0)
			{
				techniqueCount++;
				techniques[key] = tech;
				LocateShaders(tech,false);
			}
			else if(overrideTechniques.count(key) == 0 && techniques[key].pAddress != tech.pAddress)
			{
				overrideTechniqueCount++;
				overrideTechniques[key] = tech;
				LocateShaders(tech,true);
			}

			
		}
	}
	return 0;
};

int LocateAssets()
{
	//
	// Parse The TechSets first (techniques, shaders, etc have ptrs in the techsets)
	//
	ForeignPointer<unsigned __int16> f_db_hashTable((unsigned __int16*)db_hashTable);
	ForeignPointer<XAssetEntryPoolEntry> f_assetEntryPool(g_assetEntryPool);

	SIZE_T numofbytesread = 0;

	unsigned int assetCount = 0;
	for(unsigned int hashIndex = 0; hashIndex < 0x8000; hashIndex++)
	{
		__int16 nextAssetEntryIndex = 0;
		for(int assetEntryIndex = f_db_hashTable[hashIndex]; assetEntryIndex; assetEntryIndex = nextAssetEntryIndex)
		{
			ForeignPointer<XAssetEntry> assetEntry((XAssetEntry*)(g_assetEntryPool + assetEntryIndex));
			nextAssetEntryIndex = f_assetEntryPool[assetEntryIndex].entry.nextHash;
			if(f_assetEntryPool[assetEntryIndex].entry.asset.type ==  ASSET_TYPE_TECHNIQUE_SET)
			{
				assetCount++;
				ForeignPointer<XZoneName> zoneName(g_zoneNames + assetEntry->zoneIndex);
				std::string key = DB_GetXAssetName((XAsset*)assetEntry.pAddress);

				if(techsets.count(key) == 0)
				{
					techsetCount++;
					techsets[key] = ForeignPointer<MaterialTechniqueSet>(assetEntry->asset.header.techniqueSet);

					LocateTechnique(techsets[key]);
				}

				ForeignPointer<XAssetEntry> overrideAssetEntry;
				for (int overrideAssetEntryIndex = assetEntry->nextOverride; overrideAssetEntryIndex; overrideAssetEntryIndex = overrideAssetEntry->nextOverride)
				{
					overrideAssetEntry = ForeignPointer<XAssetEntry>((XAssetEntry*)(g_assetEntryPool + overrideAssetEntryIndex));
					++assetCount;
					ForeignPointer<XZoneName> overrideZoneName(g_zoneNames + overrideAssetEntry->zoneIndex);
					key = DB_GetXAssetName((XAsset*)overrideAssetEntry.pAddress);

					if(overrideTechsets.count(key) == 0)
					{
						overrideTechsetCount++;
						overrideTechsets[key] = ForeignPointer<MaterialTechniqueSet>(overrideAssetEntry->asset.header.techniqueSet);
					}
				}
			}
		}
	}

	return 0;
}

int DumpAssets(s_dumpSettings dumpSettings)
{
	printf("Locating Assets...	");
	LocateAssets();
	printf("Done!\n\n");

	printf("Found %d Techsets (%d overrides)\n",techsetCount,overrideTechsetCount); 
	printf("Found %d Techniques (%d overrides)\n",techniqueCount,overrideTechniqueCount); 
	printf("Found %d Vertex Shaders (%d overrides)\n",vShaderCount,overrideVShaderCount); 
	printf("Found %d Pixel Shaders (%d overrides)\n",pShaderCount,overridePShaderCount); 

	return 0;
}