#pragma once
#include "gdtasset.h"

class AIType : public GDTAsset
{
private:
	static const char* gdf;

	char* accuracy;
	char* animTree;
	char* character[8];
	char* comments;
	char* csvInclude;
	double engageMaxDist;
	double engageMaxFalloffDist;
	double engageMinDist;
	double engageMinFalloffDist;
	int grenadeAmmo;
	char* grenadeWeapon;
	double health;
	char* radiantModel;
	char* secondaryweapon;
	char* sidearm;
	char* team;
	char* type;
	char* weapon;

public:
	AIType();
	~AIType();

	virtual int Write(FILE* f);
};