#include "stdafx.h"

std::unordered_map<std::string, unsigned int> gPerkList(
{
	{ "specialty_longersprint",				PERK_LONGERSPRINT },
	{ "specialty_unlimitedsprint",			PERK_UNLIMITEDSPRINT },
	{ "specialty_scavenger",				PERK_SCAVENGER },
	{ "specialty_fastreload",				PERK_FASTRELOAD },
	{ "specialty_bulletdamage",				PERK_BULLETDAMAGE },
	{ "specialty_bulletaccuracy",			PERK_BULLETACCURACY },
	{ "specialty_flakjacket",				PERK_FLAKJACKET },
	{ "specialty_armorvest",				PERK_ARMORVEST },
	{ "specialty_quickrevive",				PERK_QUICKREVIVE },
	{ "specialty_altmelee",					PERK_ALTMELEE },
	{ "specialty_rof",						PERK_RATEOFFIRE },
	{ "specialty_extraammo",				PERK_EXTRAAMMO },
	{ "specialty_endurance",				PERK_ENDURANCE },
	{ "specialty_deadshot",					PERK_DEADSHOT },
	{ "specialty_additionalprimaryweapon",	PERK_ADDITIONALPRIMARYWEAPON },
	{ "specialty_fastswitch",				PERK_FASTSWITCH },
	{ "specialty_stockpile",				PERK_STOCKPILE },
	{ "specialty_fastads",					PERK_FASTADS },
	{ "specialty_fastoffhand",				PERK_FASTOFFHAND },
	{ "specialty_fastsprintrecovery",		PERK_FASTSPRINTRECOVERY },
	{ "specialty_fastmelee",				PERK_FASTMELEE },
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