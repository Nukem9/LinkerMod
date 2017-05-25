#include "stdafx.h"

// /bgame/bg_weapons_ammo.cpp:194
int BG_GetSharedAmmoCapSize(unsigned int capIndex)
{
	if (capIndex < bg_numSharedAmmoCaps)
		return bg_sharedAmmoCaps[capIndex]->iSharedAmmoCap;

	ASSERT_MSG(capIndex < bg_numSharedAmmoCaps, "capIndex doesn't index bg_numSharedAmmoCaps");
	return 0;
}

// /bgame/bg_weapons_ammo.cpp:257
AmmoClip *BG_GetAmmoClip(playerState_s *ps, int clipIndex)
{
	ASSERT(ps);

	for (int slot = 0; slot < ARRAYSIZE(ps->ammoInClip); slot++)
	{
		if (ps->ammoInClip[slot].clipIndex == clipIndex)
			return &ps->ammoInClip[slot];
	}

	return nullptr;
}

// /bgame/bg_weapons_ammo.cpp:272
AmmoClip *BG_GetFreeAmmoClip(playerState_s *ps, int clipIndex)
{
	ASSERT(ps);

	for (int slot = 0; slot < ARRAYSIZE(ps->ammoInClip); slot++)
	{
		// If we find a free/empty slot, zero it and return
		if (!ps->ammoInClip[slot].clipIndex)
		{
			memset(&ps->ammoInClip[slot], 0, sizeof(AmmoClip));
			return &ps->ammoInClip[slot];
		}
	}

	for (int slot = 0; slot < ARRAYSIZE(ps->ammoInClip); slot++)
	{
		AmmoClip *clip = &ps->ammoInClip[slot];

		ASSERT(clip->clipIndex != 0);

		bool matched = false;
		for (int weaponSlot = 0; weaponSlot < ARRAYSIZE(ps->heldWeapons); weaponSlot++)
		{
			if (BG_GetWeaponVariantDef(ps->heldWeapons[weaponSlot].weapon)->iClipIndex == clip->clipIndex)
			{
				Com_PrintError(17, "%i ammo for %s\n", clip->count, BG_WeaponName(ps->heldWeapons[weaponSlot].weapon));
				matched = true;
				break;
			}
		}

		if (!matched)
			Com_PrintError(17, "Found ammo clip %i that we do not have a weapon for\n", slot);
	}

	return nullptr;
}

// /bgame/bg_weapons_ammo.cpp:318
AmmoClip *BG_AddAmmoClip(playerState_s *ps, int clipIndex)
{
	ASSERT(ps);

	AmmoClip *clip = BG_GetAmmoClip(ps, clipIndex);

	// Wasn't found, find the next free ammo clip index
	if (!clip)
	{
		clip = BG_GetFreeAmmoClip(ps, clipIndex);
		clip->clipIndex = clipIndex;
	}

	return clip;
}

// /bgame/bg_weapons_ammo.cpp:334
void BG_AddAmmoToClip(playerState_s *ps, int clipIndex, int amount)
{
	ASSERT(ps);

	AmmoClip *clip = BG_AddAmmoClip(ps, clipIndex);
	ASSERT_MSG(clip, va("Tried to add ammo for clipIndex %i but there are no free available clips", clipIndex));

	if ((amount + clip->count) <= 0)
		clip->count = 0;
	else
		clip->count += amount;

	// Stockpile perk: the player can have one extra regular or special grenade
	for (int slot = 0; slot < ARRAYSIZE(ps->heldWeapons); slot++)
	{
		WeaponVariantDef *weapVariantDef = BG_GetWeaponVariantDef(ps->heldWeapons[slot].weapon);

		// We only care about grenades for now (max ammo: 3 or 4)
		if (weapVariantDef->weapDef->weapType != WEAPTYPE_GRENADE)
			continue;

		if (weapVariantDef->iClipIndex != clipIndex)
			continue;

		if (weapVariantDef->iClipSize < 3)
			continue;

		// Perk enabled:  clipMax
		// Perk disabled: clipMax - 1
		int maximum = weapVariantDef->iClipSize;

		if (!BG_HasPerk(ps->perks, PERK_STOCKPILE))
			maximum -= 1;

		clip->count = min(clip->count, maximum);
		break;
	}
}

// /bgame/bg_weapons_ammo.cpp:470
int BG_GetAmmoPlayerMax(playerState_s *ps, unsigned int weaponIndex, unsigned int weaponIndexToSkip)
{
	WeaponDef *weapDef = BG_GetWeaponDef(weaponIndex);
	WeaponVariantDef *weapVarDef = BG_GetWeaponVariantDef(weaponIndex);

	if (weapDef->iSharedAmmoCapIndex >= 0)
		return BG_GetSharedAmmoCapSize(weapDef->iSharedAmmoCapIndex);

	if (BG_WeaponIsClipOnly(weaponIndex))
		return BG_GetClipSize(weaponIndex);

	int total = 0;
	for (int slot = 0; slot < ARRAYSIZE(ps->heldWeapons); slot++)
	{
		int thisWeapIdx = ps->heldWeapons[slot].weapon;

		if (thisWeapIdx == weaponIndexToSkip)
			continue;

		WeaponDef *thisWeapDef = BG_GetWeaponDef(thisWeapIdx);

		if (BG_GetWeaponVariantDef(thisWeapIdx)->iAmmoIndex == weapVarDef->iAmmoIndex &&
			(thisWeapDef->inventoryType != WEAPINVENTORY_ALTMODE || BG_GetWeaponVariantDef(thisWeapIdx)->altWeaponIndex != thisWeapIdx))
		{
			if (thisWeapDef->iSharedAmmoCapIndex >= 0)
				return BG_GetSharedAmmoCapSize(thisWeapDef->iSharedAmmoCapIndex);

			total += BG_GetMaxAmmo(thisWeapIdx);

			// Stockpile perk: all of the player's weapons can store two extra magazines
			if (BG_HasPerk(ps->perks, PERK_STOCKPILE))
				total += (BG_GetClipSize(thisWeapIdx) * 2);
		}
	}

	return total;
}

// /bgame/bg_weapons_ammo.cpp:670
int BG_WeaponIsClipOnly(int weapon)
{
	return BG_GetWeaponDef(weapon)->bClipOnly;
}

// /bgame/bg_weapons_ammo.cpp:796
void PM_WeaponUseAmmo(playerState_s *ps, int wp, int amount)
{
	if (player_sustainAmmo->current.enabled)
		return;

	int usedAmmo = min(BG_GetAmmoInClip(ps, wp), amount);
	BG_AddAmmoToClip(ps, BG_GetWeaponVariantDef(wp)->iClipIndex, -usedAmmo);
}

// /bgame/bg_weapons_ammo.cpp:805
int PM_WeaponAmmoAvailable(playerState_s *ps)
{
	return BG_GetAmmoInClip(ps, PM_GetWeaponIndexForHand(ps));
}