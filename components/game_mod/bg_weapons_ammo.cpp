#include "stdafx.h"

// /bgame/bg_weapons_ammo.cpp:257
AmmoClip *BG_GetAmmoClip(playerState_s *ps, int clipIndex)
{
	ASSERT(ps);

	for (int slot = 0; slot < 15; ++slot)
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

	for (int slot = 0; slot < 15; ++slot)
	{
		// If we find a free/empty slot, zero it and return
		if (!ps->ammoInClip[slot].clipIndex)
		{
			memset(&ps->ammoInClip[slot], 0, sizeof(AmmoClip));
			return &ps->ammoInClip[slot];
		}
	}

	for (int slot = 0; slot < 15; ++slot)
	{
		AmmoClip *clip = &ps->ammoInClip[slot];

		ASSERT(clip->clipIndex != 0);

		bool matched = false;
		for (int weaponSlot = 0; weaponSlot < 15; ++weaponSlot)
		{
			if (BG_GetWeaponVariantDef(ps->heldWeapons[weaponSlot].weapon)->iClipIndex == clip->clipIndex)
			{
				Com_PrintError(17, "%i ammo for %s\n", clip->count, BG_WeaponName(ps->heldWeapons[weaponSlot].weapon));
				matched = true;
				break;
			}
		}

		if (!matched)
			Com_PrintError(17, "found ammo clip %i that we do not have a weapon for\n", slot);
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