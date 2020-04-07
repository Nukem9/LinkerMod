#pragma once

enum
{
	PERK_LONGERSPRINT = 0,
	PERK_UNLIMITEDSPRINT = 1,
	PERK_SCAVENGER = 2,
	PERK_FASTRELOAD = 3,
	PERK_BULLETDAMAGE = 4,
	PERK_BULLETACCURACY = 5,
	PERK_FLAKJACKET = 6,
	PERK_ARMORVEST = 7,
	PERK_QUICKREVIVE = 8,
	PERK_ALTMELEE = 9,
	PERK_RATEOFFIRE = 10,
	PERK_EXTRAAMMO = 11,
	PERK_ENDURANCE = 12,
	PERK_DEADSHOT = 13,
	PERK_ADDITIONALPRIMARYWEAPON = 14,

	// Begin custom perks
	PERK_FASTSWITCH = 15,
	PERK_STOCKPILE = 16,
	PERK_FASTADS = 17,
	PERK_FASTOFFHAND = 18,
	PERK_FASTSPRINTRECOVERY = 19,
	PERK_FASTMELEE = 20,

	PERK_COUNT,
	PERK_UNKNOWN = PERK_COUNT,
	ORIGINAL_PERK_COUNT = 15,
};

//
// NOTE: THIS MUST MATCH THE PERK BIT CODE IN DLLMAIN
//
// Vanilla calculation:    (3 << 2 * perkIndex)   -- 2 bits per perk
// Newer perk calculation: (1 << (perkIndex))     -- 1 bit per perk

inline unsigned int BG_PERK_BITS(int x)
{
	return 1u << x;
}

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