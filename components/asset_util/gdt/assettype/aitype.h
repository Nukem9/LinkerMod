#pragma once
#include "gdtasset.h"

#define AITYPE_CHARACTERCOUNT 8

class AIType : public GDTAsset
{
private:
	static const char* gdf;

	std::string accuracy;
	std::string animTree;
	std::string character[AITYPE_CHARACTERCOUNT];
	std::string comments;
	std::string csvInclude;
	std::string engageMaxDist;
	std::string engageMaxFalloffDist;
	std::string engageMinDist;
	std::string engageMinFalloffDist;
	std::string grenadeAmmo;
	std::string grenadeWeapon;
	std::string health;
	std::string radiantModel;
	std::string secondaryweapon;
	std::string sidearm;
	std::string team;
	std::string type;
	std::string weapon;

public:
	AIType();
	~AIType();

	int ExtractFromGSC(const char* qpath);
	virtual int Write(FILE* f);
};