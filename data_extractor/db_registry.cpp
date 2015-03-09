#include "db_registry.h"
#include "global.h"
#include "ForeignPointer.h"

const char* g_assetNames[] = 
{
	"xmodelpieces",
	"physpreset",
	"physconstraints",
	"destructibledef",
	"xanim",
	"xmodel",
	"material",
	"techset",
	"image",
	"sound",
	"sound_patch",
	"col_map_sp",
	"col_map_mp",
	"com_map",
	"game_map_sp",
	"game_map_mp",
	"map_ents",
	"gfx_map",
	"lightdef",
	"ui_map",
	"font",
	"menufile",
	"menu",
	"localize",
	"weapon",
	"weapondef",
	"weaponvariant",
	"snddriverglobals",
	"fx",
	"impactfx",
	"aitype",
	"mptype",
	"mpbody",
	"mphead",
	"character",
	"xmodelalias",
	"rawfile",
	"stringtable",
	"packindex",
	"xGlobals",
	"ddl",
	"glasses",
	"emblemset"
};

const char * DB_GetXAssetTypeName(int type)
{
	return g_assetNames[type];
}

unsigned int DB_HashForName(const char *name, XAssetType type)
{
	unsigned int hash = type;
	for(const char * pos = name; *pos; ++pos)
	{
		int c = tolower(*pos);
		if ( c == '\\' )
			c = '/';
		hash = (hash << 16) + c + (hash << 6) - hash;
	}
	return hash;
}

const char* DB_GetXAssetName(XAsset *asset)
{
	XAssetHeader header = ForeignPointer<XAsset>(asset)->header;
	SIZE_T numofbytesread = 0;

	static char str[256];

	switch(ForeignPointer<XAsset>(asset)->type)
	{
	case ASSET_TYPE_IMAGE:
		ReadProcessMemory(g_hProcess,ForeignPointer<GfxImage>(header.image)->name,str,256,&numofbytesread);
		return str;
	case ASSET_TYPE_MATERIAL:
		ReadProcessMemory(g_hProcess,ForeignPointer<Material>(header.material)->info.name,str,256,&numofbytesread);
		return str;
	case ASSET_TYPE_TECHNIQUE_SET:
		ReadProcessMemory(g_hProcess,ForeignPointer<MaterialTechniqueSet>(header.techniqueSet)->name,str,256,&numofbytesread);
		return str;
	case ASSET_TYPE_PIXEL_SHADER:
	default:
		strcpy(str,"unsupported asset type");
		return str;
	};
}
//
//XAssetEntry* DB_FindXAssetEntry(XAssetType type, const char *name)
//{
//	XAssetEntryPoolEntry *assetEntry;
//
//	for (int assetEntryIndex = db_hashTable[DB_HashForName(name, type) % 0x8000]; assetEntryIndex; assetEntryIndex = *((WORD *)&assetEntry->next + 5))
//	{
//		assetEntry = &g_assetEntryPool[assetEntryIndex];
//		if (g_assetEntryPool[assetEntryIndex].entry.asset.type == type)
//		{
//			const char *assetName = DB_GetXAssetName(&assetEntry->entry.asset);
//			if (!strcmp(assetName, name))
//				return &g_assetEntryPool[assetEntryIndex].entry;
//		}
//	}
//	return 0;
//}

#include "processHandler.h"
void DB_ListAssetPool(XAssetType type)
{
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
			if(f_assetEntryPool[assetEntryIndex].entry.asset.type == type)
			{
				assetCount++;
				ForeignPointer<XZoneName> zoneName(g_zoneNames + assetEntry->zoneIndex);
				const char* assetName = DB_GetXAssetName((XAsset*)assetEntry.pAddress);
				printf("%s,%s\n",assetName, zoneName->name);

				ForeignPointer<XAssetEntry> overrideAssetEntry;
				for (int overrideAssetEntryIndex = assetEntry->nextOverride; overrideAssetEntryIndex; overrideAssetEntryIndex = overrideAssetEntry->nextOverride)
				{
					overrideAssetEntry = ForeignPointer<XAssetEntry>((XAssetEntry*)(g_assetEntryPool + overrideAssetEntryIndex));
					++assetCount;
					ForeignPointer<XZoneName> overrideZoneName(g_zoneNames + overrideAssetEntry->zoneIndex);
					const char* overrideAssetName = DB_GetXAssetName((XAsset*)overrideAssetEntry.pAddress);
					printf( "%s,%s\n", overrideAssetName, overrideZoneName->name);
				}
			}
		}
	}
	printf("Total of %d assets in %s pool\n",assetCount, DB_GetXAssetTypeName(type));
}