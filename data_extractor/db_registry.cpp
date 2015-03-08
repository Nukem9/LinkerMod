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
	switch(ForeignPointer<XAsset>(asset)->type)
	{
	case ASSET_TYPE_MATERIAL:
		return ForeignPointer<Material>(ForeignPointer<XAsset>(asset)->header.material)->info.name;
	case ASSET_TYPE_TECHNIQUE_SET:
	case ASSET_TYPE_IMAGE:
	default:
		return "invalid asset";
	};
}

XAssetEntry* DB_FindXAssetEntry(XAssetType type, const char *name)
{
	XAssetEntryPoolEntry *assetEntry;

	for (int assetEntryIndex = db_hashTable[DB_HashForName(name, type) % 0x8000]; assetEntryIndex; assetEntryIndex = *((WORD *)&assetEntry->next + 5))
	{
		assetEntry = &g_assetEntryPool[assetEntryIndex];
		if (g_assetEntryPool[assetEntryIndex].entry.asset.type == type)
		{
			const char *assetName = DB_GetXAssetName(&assetEntry->entry.asset);
			if (!strcmp(assetName, name))
				return &g_assetEntryPool[assetEntryIndex].entry;
		}
	}
	return 0;
}

void DB_ListAssetPool(XAssetType type)
{
	int assetPoolCount = 0;
	printf("Listing assets in %s pool.\n", DB_GetXAssetTypeName(type));
	for (unsigned int hash = 0; hash < 0x8000; ++hash )
	{
		int nextAssetEntryIndex;
		for (int assetEntryIndex = db_hashTable[hash]; assetEntryIndex; assetEntryIndex = nextAssetEntryIndex )
		{
			XAssetEntry* assetEntry = &g_assetEntryPool[assetEntryIndex].entry;
			int nextAssetEntryIndex = g_assetEntryPool[assetEntryIndex].entry.nextHash;
			if ( g_assetEntryPool[assetEntryIndex].entry.asset.type == type )
			{
				++assetPoolCount;
				XZoneName* zoneName = &g_zoneNames[assetEntry->zoneIndex];
				const char* assetName = DB_GetXAssetName(&assetEntry->asset);
				printf("%s,%s\n", assetName, zoneName->name);

				XAssetEntry *overrideAssetEntry;
				for (int overrideAssetEntryIndex = assetEntry->nextOverride; overrideAssetEntryIndex; overrideAssetEntryIndex = overrideAssetEntry->nextOverride)
				{
					overrideAssetEntry = &g_assetEntryPool[overrideAssetEntryIndex].entry;
					++assetPoolCount;
					XZoneName* overrideZoneName = &g_zoneNames[g_assetEntryPool[overrideAssetEntryIndex].entry.zoneIndex];
					const char* overrideAssetName = DB_GetXAssetName(&g_assetEntryPool[overrideAssetEntryIndex].entry.asset);
					printf( "%s,%s\n", overrideAssetName, overrideZoneName->name);
				}
			}
		}
	}
	printf("Total of %d assets in %s pool\n", assetPoolCount, DB_GetXAssetTypeName(type));
}