#include "Character.h"
#include "../fs.h"
#include <regex>

const char* Character::gdf = "character.gdf";

Character::Character()
{
	// the strings should automatically be initalized with a nullstring
}

Character::~Character()
{
}

void ExtractAliasEntry(const std::string& prop, std::string& src, aliasEntry_s* dst)
{
	_ASSERT(dst != NULL);

	dst->model = ExtractStringProperty(prop, "", src);
	dst->alias = ExtractAliasProperty(prop, "", src);
}

int Character::ExtractFromGSC(const char* qpath)
{
	size_t len = FS_FileSize(qpath);
	if (len == -1)
		return -1;

	std::ifstream t(qpath);
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	//ExtractAliasEntry("bodyModel", str, &this->body);
	ExtractAliasEntry("gearModel", str, &this->gear);

	ExtractAliasEntry("hatModel", str, &this->hat);
	ExtractAliasEntry("headModel", str, &this->head);

	ExtractAliasEntry("legDmg1", str, &this->legDmg[0]);
	ExtractAliasEntry("legDmg2", str, &this->legDmg[1]);
	ExtractAliasEntry("legDmg3", str, &this->legDmg[2]);
	ExtractAliasEntry("legDmg4", str, &this->legDmg[3]);
	//Misc
	this->shadowCharacter = ExtractStringProperty("shadowCharacter", "", str);
	this->skeleton = ExtractStringProperty("skeleton", "base", str);
	ExtractAliasEntry("torsoDmg1", str, &this->torsoDmg[0]);
	ExtractAliasEntry("torsoDmg2", str, &this->torsoDmg[1]);
	ExtractAliasEntry("torsoDmg3", str, &this->torsoDmg[2]);
	ExtractAliasEntry("torsoDmg4", str, &this->torsoDmg[3]);
	ExtractAliasEntry("torsoDmg5", str, &this->torsoDmg[4]);
	this->viewmodel = ExtractStringProperty("viewmodel", "", str);
	this->voice = ExtractStringProperty("voice", "american", str);

	const char* start = FS_GetFilenameSubString(qpath);
	const char* stop = FS_GetExtensionSubString(qpath);
	this->_name = std::string(start, stop);

	return 0;
}

void WriteOptionalEntry(FILE* f, const char* name, optionalEntry_s& entry)
{
	fprintf(f, "\t\t\"%s\" \"%s\"\n", name, entry.model);
	fprintf(f, "\t\t\"%s%s\" \"%s\"\n", name, "_required", entry.required);
}

// No Underscore
void WriteAlias1(FILE* f, const char* name, aliasEntry_s& entry)
{
	fprintf(f, "\t\t\"%s\" \"%s\"\n", name, entry.model);
	fprintf(f, "\t\t\"%s%s\" \"%s\"\n", name, "Alias", entry.alias);
}

// Uses underscore
void WriteAlias2(FILE* f, const char* name, aliasEntry_s& entry)
{
	fprintf(f, "\t\t\"%s\" \"%s\"\n", name, entry.model);
	fprintf(f, "\t\t\"%s%s\" \"%s\"\n", name, "_Alias", entry.alias);
}

void WriteGibSpawn(FILE* f, const char* name, gibSpawn_s& entry)
{
	fprintf(f, "\t\t\"%s\" \"%s\"\n", name, entry.alias.model);
	fprintf(f, "\t\t\"%s%s\" \"%s\"\n", name, "_Alias", entry.alias);
}

void WriteGibSpawnTag(FILE* f, const char* name, gibSpawn_s& entry)
{
	fprintf(f, "\t\t\"%s%s\" \"%s\"\n", name, "_Alias", entry.alias);
}

int Character::Write(FILE* f)
{
	WriteAlias1(f, "body", this->body);
	WriteAlias1(f, "gear", this->gear);
	WriteGibSpawn(f, "gibSpawn1", this->gibSpawn[0]);
	WriteGibSpawn(f, "gibSpawn2", this->gibSpawn[1]);
	WriteGibSpawn(f, "gibSpawn3", this->gibSpawn[2]);
	WriteGibSpawn(f, "gibSpawn4", this->gibSpawn[3]);
	WriteGibSpawn(f, "gibSpawn5", this->gibSpawn[4]);
	fprintf(f, "\t\t\"%s\" \"%s\"\n", "gibSpawnTag1", this->gibSpawn[0].tag);
	fprintf(f, "\t\t\"%s\" \"%s\"\n", "gibSpawnTag2", this->gibSpawn[1].tag);
	fprintf(f, "\t\t\"%s\" \"%s\"\n", "gibSpawnTag3", this->gibSpawn[2].tag);
	fprintf(f, "\t\t\"%s\" \"%s\"\n", "gibSpawnTag4", this->gibSpawn[3].tag);
	fprintf(f, "\t\t\"%s\" \"%s\"\n", "gibSpawnTag5", this->gibSpawn[4].tag);
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
	fprintf(f, "\t\t\"%s\" \"%s\"\n", "shadowCharacter", this->shadowCharacter);
	fprintf(f, "\t\t\"%s\" \"%s\"\n", "skeleton", this->skeleton);
	WriteAlias2(f, "torsoDmg1", this->torsoDmg[0]);
	WriteAlias2(f, "torsoDmg2", this->torsoDmg[1]);
	WriteAlias2(f, "torsoDmg3", this->torsoDmg[2]);
	WriteAlias2(f, "torsoDmg4", this->torsoDmg[3]);
	WriteAlias2(f, "torsoDmg5", this->torsoDmg[4]);
	fprintf(f, "\t\t\"%s\" \"%s\"\n", "viewmodel", this->viewmodel);
	fprintf(f, "\t\t\"%s\" \"%s\"\n", "voice", this->voice);

	return 0;
}