#include "stdafx.h"

WeaponVariantDef *bg_weapAmmoTypes[2048];
WeaponDef *bg_sharedAmmoCaps[2048];
WeaponVariantDef *bg_weapClips[2048];

// /bgame/bg_weapons_ammo.cpp:39
void BG_SetupWeaponDefAmmoIndexes(unsigned int weapIndex, WeaponDef *weapDef, WeaponVariantDef *weapVarDef)
{
	ASSERT(weapDef);

	if (weapDef->sharedAmmo)
	{
		for (unsigned int index = 0; index < bg_numAmmoTypes; ++index)
		{
			if (!strcmp(bg_weapAmmoTypes[index]->szAmmoName, weapVarDef->szAmmoName))
			{
				weapVarDef->iAmmoIndex = index;
				return;
			}
		}
	}

	// Check if this weapon was dual-wield
	if (weapVarDef->iAmmoIndex < 0
		|| weapVarDef->iAmmoIndex >= bg_numAmmoTypes
		|| bg_weapAmmoTypes[weapVarDef->iAmmoIndex] != weapVarDef)
	{
		WeaponVariantDef *dwWeaponVarDef;
		unsigned int dwIndex;

		if (weapDef->bDualWield
			&& (dwIndex = BG_GetWeaponIndexForName(weapDef->szDualWieldWeaponName)) != 0
			&& (dwWeaponVarDef = BG_GetWeaponVariantDef(dwIndex), dwWeaponVarDef->iAmmoIndex >= 0)
			&& dwWeaponVarDef->iAmmoIndex < bg_numAmmoTypes
			&& bg_weapAmmoTypes[dwWeaponVarDef->iAmmoIndex] == dwWeaponVarDef)
		{
			weapVarDef->iAmmoIndex = dwWeaponVarDef->iAmmoIndex;
		}
		else
		{
			ASSERT_MSG(bg_numWeapClips < ARRAYSIZE(bg_weapClips), "bg_numWeapClips doesn't index ARRAY_COUNT(bg_weapClips)");

			bg_weapAmmoTypes[bg_numAmmoTypes] = weapVarDef;
			weapVarDef->iAmmoIndex = bg_numAmmoTypes++;
		}
	}
}

// /bgame/bg_weapons_ammo.cpp:86
void BG_SetupWeaponDefSharedAmmoIndexes(unsigned int weapIndex, WeaponDef *weapDef)
{
	ASSERT(weapDef);
	ASSERT(weapDef->szSharedAmmoCapName);

	weapDef->iSharedAmmoCapIndex = -1;

	if (!*weapDef->szSharedAmmoCapName)
		return;

	Com_DPrintf(17, "%s: %s\n", BG_WeaponName(weapIndex), weapDef->szSharedAmmoCapName);

	unsigned int index = 0;
	for (;; index++)
	{
		// Check if this was a new entry
		if (index >= bg_numSharedAmmoCaps)
		{
			bg_sharedAmmoCaps[index] = weapDef;
			weapDef->iSharedAmmoCapIndex = index;
			bg_numSharedAmmoCaps++;
			return;
		}

		// Duplicate entry, now check for conflicting ammo counts
		if (!_stricmp(bg_sharedAmmoCaps[index]->szSharedAmmoCapName, weapDef->szSharedAmmoCapName))
			break;
	}

	weapDef->iSharedAmmoCapIndex = index;

	if (bg_sharedAmmoCaps[index]->iSharedAmmoCap != weapDef->iSharedAmmoCap && index)
	{
		for (unsigned int otherWeapIndex = 1; otherWeapIndex < weapIndex; otherWeapIndex++)
		{
			WeaponDef *otherWeapDef = BG_GetWeaponDef(otherWeapIndex);

			if (!_stricmp(bg_sharedAmmoCaps[index]->szSharedAmmoCapName, otherWeapDef->szSharedAmmoCapName)
				&& otherWeapDef->iSharedAmmoCap == bg_sharedAmmoCaps[index]->iSharedAmmoCap)
			{
				Com_Error(ERR_DROP, "Shared ammo cap mismatch for \"%s\" shared ammo cap: '%s\" set it to %i, but \"%s\" already set it to %i.",
					weapDef->szSharedAmmoCapName,
					BG_WeaponName(weapIndex),
					weapDef->iSharedAmmoCap,
					BG_WeaponName(otherWeapIndex),
					otherWeapDef->iSharedAmmoCap);
			}
		}

		ASSERT_MSG(false, "unreachable");
	}
}

// /bgame/bg_weapons_ammo.cpp:134
void BG_SetupWeaponDefClipIndexes(WeaponDef *weapDef, WeaponVariantDef *weapVarDef)
{
	ASSERT(weapDef);

	if (weapDef->unlimitedAmmo)
	{
		// Nothing to use/draw with unlimited ammo weapons
		weapVarDef->iClipIndex = -1;
		return;
	}

	// Determine shared ammo clip index
	if (weapDef->sharedAmmo)
	{
		for (unsigned int index = 0; index < bg_numWeapClips; index++)
		{
			if (!strcmp(bg_weapClips[index]->szClipName, weapVarDef->szClipName))
			{
				weapVarDef->iClipIndex = index;
				return;
			}
		}
	}

	// Add a new entry if this was a new clip type
	if (weapVarDef->iClipIndex < 0
		|| weapVarDef->iClipIndex >= bg_numWeapClips
		|| bg_weapClips[weapVarDef->iClipIndex] != weapVarDef)
	{
		ASSERT_MSG(bg_numWeapClips < ARRAYSIZE(bg_weapClips), "bg_numWeapClips doesn't index ARRAY_COUNT(bg_weapClips)");

		bg_weapClips[bg_numWeapClips] = weapVarDef;
		weapVarDef->iClipIndex = bg_numWeapClips++;
	}
}

// /bgame/bg_weapons_ammo.cpp:170
void BG_ClearWeaponDefAmmo()
{
	WeaponVariantDef *defaultWeap = BG_LoadDefaultWeaponVariantDef();

	bg_weapAmmoTypes[0] = defaultWeap;
	bg_numAmmoTypes = 1;

	bg_sharedAmmoCaps[0] = defaultWeap->weapDef;
	bg_numSharedAmmoCaps = 1;

	bg_weapClips[0] = defaultWeap;
	bg_numWeapClips = 1;
}

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
	ASSERT_MSG_VA(clip, "Tried to add ammo for clipIndex %i but there are no free available clips", clipIndex);

	if ((amount + clip->count) <= 0)
		clip->count = 0;
	else
		clip->count += amount;

	// Stockpile perk: the player can have one extra regular or special grenade
	for (int slot = 0; slot < ARRAYSIZE(ps->heldWeapons); slot++)
	{
		WeaponVariantDef *weapVariantDef = BG_GetWeaponVariantDef(ps->heldWeapons[slot].weapon);

		// Was this one of the variants we patched (BG_SetupWeaponIndex)? If not, skip it
		if (std::find(bg_CheckedVariantDefs.begin(), bg_CheckedVariantDefs.end(), weapVariantDef) == bg_CheckedVariantDefs.end())
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

// /bgame/bg_weapons_ammo.cpp:513
bool ValueInArray(const int *weaponArray, int value)
{
	for (int slot = 0; slot < 15; slot++)
	{
		if (weaponArray[slot] == value)
			return true;
	}

	return false;
}

// /bgame/bg_weapons_ammo.cpp:525
void AddValueToArray(int *weaponArray, int value)
{
	for (int slot = 0; slot < 15; slot++)
	{
		if (weaponArray[slot])
			continue;

		weaponArray[slot] = value;
		return;
	}

	ASSERT(false);
}

// /bgame/bg_weapons_ammo.cpp:542
int BG_GetMaxPickupableAmmo(playerState_s *ps, unsigned int weaponIndex)
{
	int clipCounted[15];
	int ammoCounted[15];

	memset(ammoCounted, 0, sizeof(clipCounted));
	memset(clipCounted, 0, sizeof(ammoCounted));

	WeaponDef *weapDef = BG_GetWeaponDef(weaponIndex);

	// If this weapon uses part of the shared ammo pool
	if (weapDef->iSharedAmmoCapIndex >= 0)
	{
		int ammo = BG_GetSharedAmmoCapSize(weapDef->iSharedAmmoCapIndex);

		for (int slot = 0; slot < ARRAYSIZE(ps->heldWeapons); slot++)
		{
			unsigned int currWeap = ps->heldWeapons[slot].weapon;

			if (!currWeap)
				continue;

			WeaponDef *curWeapDef = BG_GetWeaponDef(currWeap);

			if (curWeapDef->iSharedAmmoCapIndex != weapDef->iSharedAmmoCapIndex)
				continue;

			if (BG_WeaponIsClipOnly(currWeap))
			{
				if (!ValueInArray(clipCounted, BG_ClipForWeapon(currWeap)))
				{
					AddValueToArray(clipCounted, BG_ClipForWeapon(currWeap));
					ammo -= BG_GetAmmoInClip(ps, currWeap);
				}
			}
			else
			{
				if (!ValueInArray(ammoCounted, BG_AmmoForWeapon(currWeap)))
				{
					AddValueToArray(ammoCounted, BG_AmmoForWeapon(currWeap));
					ammo -= BG_GetAmmoNotInClip(ps, currWeap);
				}
			}
		}

		return ammo;
	}

	// Simple check if this weapon has no ammo stock (clip only)
	if (BG_WeaponIsClipOnly(weaponIndex))
		return BG_GetClipSize(weaponIndex) - BG_GetAmmoInClip(ps, weaponIndex);

	// Otherwise do the normal calculation with clip/stock
	int maxAmmo = BG_GetAmmoPlayerMax(ps, weaponIndex, 0);
	return maxAmmo - BG_GetAmmoNotInClip(ps, weaponIndex);
}

// /bgame/bg_weapons_ammo.cpp:660
int BG_ClipForWeapon(int weapon)
{
	return BG_GetWeaponVariantDef(weapon)->iClipIndex;
}

// /bgame/bg_weapons_ammo.cpp:665
int BG_AmmoForWeapon(int weapon)
{
	return BG_GetWeaponVariantDef(weapon)->iAmmoIndex;
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