#include "Character.h"
#include "../gsc.h"
#include "../fs.h"

const char* Character::gdf = "character.gdf";

Character::Character()
{
	// the strings should automatically be initalized with a nullstring
}

Character::~Character()
{
}

int Character::ExtractFromGSC(const char* qpath)
{
	size_t len = FS_FileSize(qpath);
	if (len == -1)
		return -1;

	std::ifstream t(qpath);
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	//ExtractAliasEntry("bodyModel", str, &this->body);
	GSC_Character_ExtractAliasEntry("gearModel", str.c_str(), &this->gear, "");

	GSC_Character_ExtractGibSpawnEntries(str.c_str(), this->gibSpawn, 5, "");

	GSC_Character_ExtractAliasEntry("hatModel", str.c_str(), &this->hat, "");
	GSC_Character_ExtractAliasEntry("headModel", str.c_str(), &this->head, "");

	GSC_Character_ExtractAliasEntry("legDmg1", str.c_str(), &this->legDmg[0], "");
	GSC_Character_ExtractAliasEntry("legDmg2", str.c_str(), &this->legDmg[1], "");
	GSC_Character_ExtractAliasEntry("legDmg3", str.c_str(), &this->legDmg[2], "");
	GSC_Character_ExtractAliasEntry("legDmg4", str.c_str(), &this->legDmg[3], "");
	
	// misc models arent supported right now
	this->misc[0].model = "";
	this->misc[1].model = "";
	this->misc[2].model = "";

	/*Unsupported
	
	optionalEntry_s J_Backpack;
	optionalEntry_s J_ChestGear_LE;
	optionalEntry_s J_ChestGear_RI;
	optionalEntry_s J_FrontPack;
	optionalEntry_s J_FrontPackLow;
	optionalEntry_s J_GearFront_LE;
	optionalEntry_s J_GearFront_RI;
	optionalEntry_s J_GearRear_LE;
	optionalEntry_s J_GearRear_RI;
	optionalEntry_s J_GearSide_LE;
	optionalEntry_s J_GearSide_RI;
	optionalEntry_s J_GearSideRear_LE;
	optionalEntry_s J_GearSideRear_RI;
	optionalEntry_s J_Helmet;
	
	*/

	GSC_ExtractStringProperty("shadowCharacter", str.c_str(), &this->shadowCharacter, "");
	GSC_ExtractStringProperty("skeleton", str.c_str(), &this->skeleton, "base");

	GSC_Character_ExtractAliasEntry("torsoDmg1", str.c_str(), &this->torsoDmg[0], "");
	GSC_Character_ExtractAliasEntry("torsoDmg2", str.c_str(), &this->torsoDmg[1], "");
	GSC_Character_ExtractAliasEntry("torsoDmg3", str.c_str(), &this->torsoDmg[2], "");
	GSC_Character_ExtractAliasEntry("torsoDmg4", str.c_str(), &this->torsoDmg[3], "");
	GSC_Character_ExtractAliasEntry("torsoDmg5", str.c_str(), &this->torsoDmg[4], "");

	GSC_ExtractStringProperty("viewmodel", str.c_str(), &this->viewmodel, "");
	GSC_ExtractStringProperty("voice", str.c_str(), &this->voice, "american");

	const char* start = FS_GetFilenameSubString(qpath);
	const char* stop = FS_GetExtensionSubString(qpath);
	this->_name = std::string(start, stop);

	return 0;
}

void WriteOptionalEntry(FILE* f, const char* name, optionalEntry_s& entry)
{
	fprintf(f, "\t\t\"%s\" \"%s\"\n", name, entry.model.c_str());
	fprintf(f, "\t\t\"%s%s\" \"%s\"\n", name, "_required", entry.required.c_str());
}

// No Underscore
void WriteAlias1(FILE* f, const char* name, aliasEntry_s& entry)
{
	fprintf(f, "\t\t\"%s\" \"%s\"\n", name, entry.model.c_str());
	fprintf(f, "\t\t\"%s%s\" \"%s\"\n", name, "Alias", entry.alias.c_str());
}

// Uses underscore
void WriteAlias2(FILE* f, const char* name, aliasEntry_s& entry)
{
	fprintf(f, "\t\t\"%s\" \"%s\"\n", name, entry.model.c_str());
	fprintf(f, "\t\t\"%s%s\" \"%s\"\n", name, "_Alias", entry.alias.c_str());
}

void WriteGibSpawn(FILE* f, const char* name, gibSpawn_s& entry)
{
	WriteAlias2(f, name, entry.alias);
}

void WriteGibSpawnTag(FILE* f, const char* name, gibSpawn_s& entry)
{
	fprintf(f, "\t\t\"%s\" \"%s\"\n", name, entry.alias.model.c_str());
}

int Character::Write(FILE* f)
{
	fprintf(f, "\t\"%s\" ( \"%s\" )\n", this->_name.c_str(), this->gdf);
	fprintf(f, "\t{\n");

	WriteAlias1(f, "body", this->body);
	WriteAlias1(f, "gear", this->gear);
	WriteGibSpawn(f, "gibSpawn1", this->gibSpawn[0]);
	WriteGibSpawn(f, "gibSpawn2", this->gibSpawn[1]);
	WriteGibSpawn(f, "gibSpawn3", this->gibSpawn[2]);
	WriteGibSpawn(f, "gibSpawn4", this->gibSpawn[3]);
	WriteGibSpawn(f, "gibSpawn5", this->gibSpawn[4]);
	fprintf(f, "\t\t\"%s\" \"%s\"\n", "gibSpawnTag1", this->gibSpawn[0].tag.c_str());
	fprintf(f, "\t\t\"%s\" \"%s\"\n", "gibSpawnTag2", this->gibSpawn[1].tag.c_str());
	fprintf(f, "\t\t\"%s\" \"%s\"\n", "gibSpawnTag3", this->gibSpawn[2].tag.c_str());
	fprintf(f, "\t\t\"%s\" \"%s\"\n", "gibSpawnTag4", this->gibSpawn[3].tag.c_str());
	fprintf(f, "\t\t\"%s\" \"%s\"\n", "gibSpawnTag5", this->gibSpawn[4].tag.c_str());
	WriteAlias1(f, "hat", this->hat);
	WriteAlias1(f, "head", this->head);
	WriteOptionalEntry(f, "J_Backpack", this->J_Backpack);
	WriteOptionalEntry(f, "J_ChestGear_LE", this->J_ChestGear_LE);
	WriteOptionalEntry(f, "J_ChestGear_RI", this->J_ChestGear_RI);
	WriteOptionalEntry(f, "J_FrontPack", this->J_FrontPack);
	WriteOptionalEntry(f, "J_FrontPackLow", this->J_FrontPackLow);
	WriteOptionalEntry(f, "J_GearFront_LE", this->J_GearFront_LE);
	WriteOptionalEntry(f, "J_GearFront_RI", this->J_GearFront_RI);
	WriteOptionalEntry(f, "J_GearRear_LE", this->J_GearRear_LE);
	WriteOptionalEntry(f, "J_GearRear_RI", this->J_GearRear_RI);
	WriteOptionalEntry(f, "J_GearSide_LE", this->J_GearSide_LE);
	WriteOptionalEntry(f, "J_GearSide_RI", this->J_GearSide_RI);
	WriteOptionalEntry(f, "J_GearSideRear_LE", this->J_GearSideRear_LE);
	WriteOptionalEntry(f, "J_GearSideRear_RI", this->J_GearSideRear_RI);
	WriteOptionalEntry(f, "J_Helmet", this->J_Helmet);
	WriteAlias2(f, "legDmg1", this->legDmg[0]);
	WriteAlias2(f, "legDmg2", this->legDmg[1]);
	WriteAlias2(f, "legDmg3", this->legDmg[2]);
	WriteAlias2(f, "legDmg4", this->legDmg[3]);
	WriteOptionalEntry(f, "misc1", this->misc[0]);
	WriteOptionalEntry(f, "misc2", this->misc[1]);
	WriteOptionalEntry(f, "misc3", this->misc[2]);
	fprintf(f, "\t\t\"%s\" \"%s\"\n", "shadowCharacter", this->shadowCharacter.c_str());
	fprintf(f, "\t\t\"%s\" \"%s\"\n", "skeleton", this->skeleton.c_str());
	WriteAlias2(f, "torsoDmg1", this->torsoDmg[0]);
	WriteAlias2(f, "torsoDmg2", this->torsoDmg[1]);
	WriteAlias2(f, "torsoDmg3", this->torsoDmg[2]);
	WriteAlias2(f, "torsoDmg4", this->torsoDmg[3]);
	WriteAlias2(f, "torsoDmg5", this->torsoDmg[4]);
	fprintf(f, "\t\t\"%s\" \"%s\"\n", "viewmodel", this->viewmodel.c_str());
	fprintf(f, "\t\t\"%s\" \"%s\"\n", "voice", this->voice.c_str());

	fprintf(f, "\t}\n");

	return 0;
}