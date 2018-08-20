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
	PERK_FASTSWITCH = 15,
	PERK_STOCKPILE = 16,

	PERK_COUNT,
	PERK_UNKNOWN = PERK_COUNT,
	ORIGINAL_PERK_COUNT = 15,
};

static bool IS_VANILLA_CALCULATION_PERK(int x)
{
	return (x == PERK_LONGERSPRINT || x == PERK_UNLIMITEDSPRINT || x == PERK_FLAKJACKET || x == PERK_ENDURANCE || x == PERK_DEADSHOT);
}

// Vanilla calculation:    (3 << 2 * perkIndex)                            -- 2 bits per perk
// Newer perk calculation: (1 << (vanillaPerksBelowPerkIndex + perkIndex)) -- 1 bit per perk

// 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00 (bit number)
// 26 25 24 23 13 13 12 12 22 21 20 19 18 17 16 15 14 11 06 06 10 09 08 07 05 04 03 02 01 01 00 00 (perk number)
//  0  0  0  0  1  1  1  1  0  0  0  0  0  0  0  0  0  0  1  1  0  0  0  0  0  0  0  0  1  1  1  1 (1 = vanilla calculation bit, 0 = new calculation bit)
// Vanilla calculation perks: still use vanilla engine code
// Vanilla calculation perks: bit number starts at (perk number * 2)

inline unsigned int BG_PERK_BITS(int x)
{
	//return ((x >= 15) ? (1UL << (15 + x)) : (3UL << 2 * x));

	if (IS_VANILLA_CALCULATION_PERK(x))
	{
		return 3UL << 2 * x;
	}
	else
	{
		int vanillaPerksBelowPerkIndex = 0;
		for (int i = 0; i < x; i++)
		{
			if (IS_VANILLA_CALCULATION_PERK(i))
			{
				if (x + vanillaPerksBelowPerkIndex > i * 2)
				{
					vanillaPerksBelowPerkIndex++;
				}
				else
				{
					vanillaPerksBelowPerkIndex--;
				}
			}
		}

		return 1UL << (vanillaPerksBelowPerkIndex + x);
	}
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