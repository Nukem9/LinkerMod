#pragma once
#include "gdtasset.h"

struct optionalEntry_s
{
	std::string model;
	std::string required;
};

struct aliasEntry_s
{
	std::string model;
	std::string alias;
};

struct gibSpawn_s
{
	aliasEntry_s alias;
	std::string tag;
};

class Character : public GDTAsset
{
private:
	static const char* gdf;

	aliasEntry_s body;
	aliasEntry_s gear;
	gibSpawn_s gibSpawn[5];
	aliasEntry_s hat;
	aliasEntry_s head;
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
	aliasEntry_s legDmg[4];
	optionalEntry_s misc[3];
	std::string shadowCharacter;
	std::string skeleton;
	aliasEntry_s torsoDmg[5];
	std::string viewmodel;
	std::string voice;

public:
	Character();
	~Character();

	int ExtractFromGSC(const char* qpath);
	virtual int Write(FILE* f);
};