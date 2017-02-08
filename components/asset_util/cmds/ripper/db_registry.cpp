#include "db_registry.h"
#include "foreign_ptr.h"
#include "../../common/io.h"

static const char* g_assetNames[] =
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
	for (const char * pos = name; *pos; ++pos)
	{
		int c = tolower(*pos);
		if (c == '\\')
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

	//switch (ForeignPointer<XAsset>(asset)->type)
	//{
	//default:
		strcpy_s(str, "unsupported asset type");
		return str;
	//};
}

int DB_EnumAssetPool(XAssetType type, asset_callback_t assetCallback_f, asset_callback_t assetOverrideCallback_f, const char* zone)
{
	ForeignPointer<unsigned __int16>& db_hashTable = g_process.db_hashTable;
	ForeignPointer<XAssetEntryPoolEntry>& db_assetEntryPool = g_process.db_assetEntryPool;
	ForeignPointer<XZoneName>& db_zoneNames = g_process.db_zoneNames;

	SIZE_T numofbytesread = 0;

	unsigned int assetCount = 0;
	for (unsigned int hashIndex = 0; hashIndex < 0x8000; hashIndex++)
	{
		__int16 nextAssetEntryIndex = 0;
		for (int assetEntryIndex = db_hashTable[hashIndex]; assetEntryIndex; assetEntryIndex = nextAssetEntryIndex)
		{
			XAssetEntry assetEntry = db_assetEntryPool[assetEntryIndex].entry;

			nextAssetEntryIndex = assetEntry.nextHash;
			if (assetEntry.asset.type != type)
				continue;

			if (assetCallback_f)
			{
				XAsset* assetOffset = (XAsset*)(db_assetEntryPool.pAddress + assetEntryIndex);
				ForeignPointer<XZoneName> zoneName(db_zoneNames.pAddress + assetEntry.zoneIndex);
				if (zone == NULL || (zone != NULL && _stricmp(zoneName->name, zone) == 0))
				{
					ForeignPointer<XAsset> asset(assetOffset);
					if (assetCallback_f(asset, zoneName) == 0)
						assetCount++;
				}
			}

			if (!assetOverrideCallback_f)
				continue;

			XAssetEntry overrideAssetEntry;
			for (int overrideAssetEntryIndex = assetEntry.nextOverride; overrideAssetEntryIndex; overrideAssetEntryIndex = overrideAssetEntry.nextOverride)
			{
				overrideAssetEntry = db_assetEntryPool[overrideAssetEntryIndex].entry;

				XAsset* overrideAssetOffset = (XAsset*)(db_assetEntryPool.pAddress + overrideAssetEntryIndex);
				ForeignPointer<XZoneName> overrideZoneName(db_zoneNames.pAddress + overrideAssetEntry.zoneIndex);
				if (zone == NULL || (zone != NULL && _stricmp(overrideZoneName->name, zone) == 0))
				{
					ForeignPointer<XAsset> overrideAsset(overrideAssetOffset);
					if (assetOverrideCallback_f(overrideAsset, overrideZoneName) == 0)
						assetCount++;
				}
			}
		}
	}

	return assetCount;
}

int DB_EnumAssetPoolEx(XAssetType type, asset_callback_ex_t assetCallback_f, asset_callback_ex_t assetOverrideCallback_f, void* data, const char* zone)
{
	ForeignPointer<unsigned __int16>& db_hashTable = g_process.db_hashTable;
	ForeignPointer<XAssetEntryPoolEntry>& db_assetEntryPool = g_process.db_assetEntryPool;
	ForeignPointer<XZoneName>& db_zoneNames = g_process.db_zoneNames;

	SIZE_T numofbytesread = 0;

	unsigned int assetCount = 0;
	for (unsigned int hashIndex = 0; hashIndex < 0x8000; hashIndex++)
	{
		__int16 nextAssetEntryIndex = 0;
		for (int assetEntryIndex = db_hashTable[hashIndex]; assetEntryIndex; assetEntryIndex = nextAssetEntryIndex)
		{
			XAssetEntry assetEntry = db_assetEntryPool[assetEntryIndex].entry;

			nextAssetEntryIndex = assetEntry.nextHash;
			if (assetEntry.asset.type != type)
				continue;

			if (assetCallback_f)
			{
				XAsset* assetOffset = (XAsset*)(db_assetEntryPool.pAddress + assetEntryIndex);
				ForeignPointer<XZoneName> zoneName(db_zoneNames.pAddress + assetEntry.zoneIndex);
				if (zone == NULL || (zone != NULL && _stricmp(zoneName->name, zone) == 0))
				{
					ForeignPointer<XAsset> asset(assetOffset);
					if (assetCallback_f(asset, zoneName, data) == 0)
						assetCount++;
				}
			}

			if (!assetOverrideCallback_f)
				continue;

			XAssetEntry overrideAssetEntry;
			for (int overrideAssetEntryIndex = assetEntry.nextOverride; overrideAssetEntryIndex; overrideAssetEntryIndex = overrideAssetEntry.nextOverride)
			{
				overrideAssetEntry = db_assetEntryPool[overrideAssetEntryIndex].entry;

				XAsset* overrideAssetOffset = (XAsset*)(db_assetEntryPool.pAddress + overrideAssetEntryIndex);
				ForeignPointer<XZoneName> overrideZoneName(db_zoneNames.pAddress + overrideAssetEntry.zoneIndex);
				if (zone == NULL || (zone != NULL && _stricmp(overrideZoneName->name, zone) == 0))
				{
					ForeignPointer<XAsset> overrideAsset(overrideAssetOffset);
					if (assetOverrideCallback_f(overrideAsset, overrideZoneName, data) == 0)
						assetCount++;
				}
			}
		}
	}

	return assetCount;
}

void* DB_FindSingletonAssetForType(XAssetType type)
{
	ForeignPointer<unsigned __int16>& db_hashTable = g_process.db_hashTable;
	ForeignPointer<XAssetEntryPoolEntry>& db_assetEntryPool = g_process.db_assetEntryPool;
	ForeignPointer<XZoneName>& db_zoneNames = g_process.db_zoneNames;

	SIZE_T numofbytesread = 0;

	unsigned int assetCount = 0;
	for (unsigned int hashIndex = 0; hashIndex < 0x8000; hashIndex++)
	{
		__int16 nextAssetEntryIndex = 0;
		for (int assetEntryIndex = db_hashTable[hashIndex]; assetEntryIndex; assetEntryIndex = nextAssetEntryIndex)
		{
			XAssetEntry assetEntry = db_assetEntryPool[assetEntryIndex].entry;

			nextAssetEntryIndex = assetEntry.nextHash;
			if (assetEntry.asset.type != type)
				continue;

			return assetEntry.asset.header.data;
		}
	}

	return NULL;
}

int DB_ListAssetPool_AssetCallback(ForeignPointer<XAsset>& asset, ForeignPointer<XZoneName>& zoneName)
{
	const char* assetName = DB_GetXAssetName(asset.pAddress);
	Con_Print("%s,%s\n", assetName, zoneName->name);
	return 0;
}

void DB_ListAssetPool(XAssetType type, const char* zone)
{
	int count = DB_EnumAssetPool(type, DB_ListAssetPool_AssetCallback, DB_ListAssetPool_AssetCallback, zone);
	printf("Total of %d assets in %s pool\n", count, DB_GetXAssetTypeName(type));
}
