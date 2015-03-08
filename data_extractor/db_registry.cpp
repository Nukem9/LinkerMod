#include "db_registry.h"
#include <Windows.h>

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

