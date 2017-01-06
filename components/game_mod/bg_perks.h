#pragma once

#define PERK_DOUBLETAP 10

// /game/bg_perks.h:135
static bool BG_HasPerk(const unsigned int *perks, unsigned int perkIndex)
{
	ASSERT(perks);
	ASSERT(perkIndex >= 0 && perkIndex < 15);

	return (*perks & (3 << 2 * perkIndex)) != 0;
}