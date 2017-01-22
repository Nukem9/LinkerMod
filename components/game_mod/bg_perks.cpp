#include "stdafx.h"

std::unordered_map<std::string, unsigned int> gPerkList(
{
	{ "specialty_longersprint",				0 },
	{ "specialty_unlimitedsprint",			1 },
	{ "specialty_scavanger",				2 },
	{ "specialty_fastreload",				PERK_FASTRELOAD },
	{ "specialty_bulletdamage",				4 },
	{ "specialty_bulletaccuracy",			5 },
	{ "specialty_flakjacket",				6 },
	{ "specialty_armorvest",				7 },
	{ "specialty_quickrevive",				8 },
	{ "specialty_altmelee",					9 },
	{ "specialty_rof",						PERK_RATEOFFIRE },
	{ "specialty_extraammo",				11 },
	{ "specialty_endurance",				12 },
	{ "specialty_deadshot",					13 },
	{ "specialty_additionalprimaryweapon",	14 },
	{ "specialty_fastswitch",				PERK_FASTSWITCH },
});

// /game/bg_perks.cpp:129
unsigned int BG_GetPerkIndexForName(const char *perkName)
{
	auto found = gPerkList.find(perkName);

	if (found != gPerkList.end())
		return found->second;

	return PERK_UNKNOWN;
}

// /game/bg_perks.cpp:143
const char *BG_GetPerkNameForIndex(unsigned int perkIndex)
{
	ASSERT(perkIndex < PERK_COUNT);

	for(const auto& entry : gPerkList)
	{
		if (entry.second == perkIndex)
			return entry.first.c_str();
	}

	return "<error>";
}