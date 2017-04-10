#pragma once

AmmoClip *BG_GetAmmoClip(playerState_s *ps, int clipIndex);
AmmoClip *BG_GetFreeAmmoClip(playerState_s *ps, int clipIndex);
AmmoClip *BG_AddAmmoClip(playerState_s *ps, int clipIndex);
void BG_AddAmmoToClip(playerState_s *ps, int clipIndex, int amount);
void PM_WeaponUseAmmo(playerState_s *ps, int wp, int amount);
int PM_WeaponAmmoAvailable(playerState_s *ps);

// /bgame/bg_weapons_ammo.h:118
static int BG_GetAmmoInClipForWeaponDef(playerState_s *ps, WeaponVariantDef *weapVarDef)
{
	ASSERT(ps);
	ASSERT(weapVarDef);

	for (int i = 0; i < 15; ++i)
	{
		if (ps->ammoInClip[i].clipIndex == weapVarDef->iClipIndex)
			return ps->ammoInClip[i].count;
	}

	return 0;
}

// /bgame/bg_weapons_ammo.h:129
static int BG_GetAmmoInClip(playerState_s *ps, unsigned int weaponIndex)
{
	ASSERT(ps);

	return BG_GetAmmoInClipForWeaponDef(ps, BG_GetWeaponVariantDef(weaponIndex));
}