#pragma once

// Vanilla calculation:    (3 << 2 * perkIndex)     -- 2 bits per perk
// Newer perk calculation: (1 << (15 + perkIndex))  -- 1 bit per perk
inline unsigned int BG_PERK_BITS(int x)
{
	return ((x >= 15) ? (1UL << (15 + x)) : (3UL << 2 * x));
}

enum
{
	PERK_FASTRELOAD = 3,		// (3 << (2 * 3))   == 0xC0
	PERK_RATEOFFIRE = 10,		// (3 << (2 * 10))  == 0x300000
	PERK_FASTSWITCH = 15,		// (1 << (15 + 15)) == 0x40000000
	PERK_STOCKPILE = 16,		// (1 << (15 + 16)) == 0x80000000

	PERK_COUNT,					// Game original: 15
	PERK_UNKNOWN = PERK_COUNT,	//
};

// /game/bg_perks.h:135
static bool BG_HasPerk(const unsigned int *perks, unsigned int perkIndex)
{
	ASSERT(perks);
	ASSERT(perkIndex >= 0 && perkIndex < PERK_COUNT);

	return (perks[0] & BG_PERK_BITS(perkIndex)) != 0;
}

// /game/bg_perks.h:144
static void BG_SetPerk(unsigned int *perks, unsigned int perkIndex)
{
	ASSERT(perks);
	ASSERT(perkIndex >= 0 && perkIndex < PERK_COUNT);

	perks[0] |= BG_PERK_BITS(perkIndex);
}

// /game/bg_perks.h:159
static void BG_UnsetPerk(unsigned int *perks, unsigned int perkIndex)
{
	ASSERT(perks);
	ASSERT(perkIndex >= 0 && perkIndex < PERK_COUNT);

	perks[0] &= ~BG_PERK_BITS(perkIndex);
}

// /game/bg_perks.h:174
static void BG_ClearPerks(unsigned int *perks)
{
	ASSERT(perks);

	perks[0] = 0;
}

// /game/bg_perks.h:???
static bool BG_WeaponHasPerk(WeaponDef *weapDef, unsigned int perkIndex)
{
	// This is only implemented in BO2
	return false;
}

unsigned int BG_GetPerkIndexForName(const char *perkName);
const char *BG_GetPerkNameForIndex(unsigned int perkIndex);