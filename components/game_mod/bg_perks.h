#pragma once

enum
{
	PERK_FASTRELOAD = 3,
	PERK_RATEOFFIRE = 10,
	PERK_FASTSWITCH = 15,

	PERK_COUNT = 16,			// Game original: 15
	PERK_UNKNOWN = PERK_COUNT,	//
};

// /game/bg_perks.h:135
static bool BG_HasPerk(const unsigned int *perks, unsigned int perkIndex)
{
	ASSERT(perks);
	ASSERT(perkIndex >= 0 && perkIndex < PERK_COUNT);

	return (perks[0] & (3 << 2 * perkIndex)) != 0;
}

// /game/bg_perks.h:144
static void BG_SetPerk(unsigned int *perks, unsigned int perkIndex)
{
	ASSERT(perks);
	ASSERT(perkIndex >= 0 && perkIndex < PERK_COUNT);

	perks[0] |= (3 << 2 * perkIndex);
}

// /game/bg_perks.h:159
static void BG_UnsetPerk(unsigned int *perks, unsigned int perkIndex)
{
	ASSERT(perks);
	ASSERT(perkIndex >= 0 && perkIndex < PERK_COUNT);

	perks[0] &= ~(3 << 2 * perkIndex);
}

// /game/bg_perks.h:174
static void BG_ClearPerks(unsigned int *perks)
{
	ASSERT(perks);

	perks[0] = 0;
}

unsigned int BG_GetPerkIndexForName(const char *perkName);
const char *BG_GetPerkNameForIndex(unsigned int perkIndex);