#pragma once

static WeaponVariantDef **bg_weapAmmoTypes = (WeaponVariantDef **)0x00BDF998;
static WeaponDef **bg_sharedAmmoCaps = (WeaponDef **)0x00BDF798;
static WeaponVariantDef **bg_weapClips = (WeaponVariantDef **)0x00BDFB98;

static unsigned int& bg_numAmmoTypes = *(unsigned int *)0x00BDF794;
static unsigned int& bg_numSharedAmmoCaps = *(unsigned int *)0x00BDFD98;
static unsigned int& bg_numWeapClips = *(unsigned int *)0x00BDF790;

void BG_SetupWeaponDefAmmoIndexes(unsigned int weapIndex, WeaponDef *weapDef, WeaponVariantDef *weapVarDef);
void BG_SetupWeaponDefSharedAmmoIndexes(unsigned int weapIndex, WeaponDef *weapDef);
void BG_SetupWeaponDefClipIndexes(WeaponDef *weapDef, WeaponVariantDef *weapVarDef);
int BG_GetSharedAmmoCapSize(unsigned int capIndex);
AmmoClip *BG_GetAmmoClip(playerState_s *ps, int clipIndex);
AmmoClip *BG_GetFreeAmmoClip(playerState_s *ps, int clipIndex);
AmmoClip *BG_AddAmmoClip(playerState_s *ps, int clipIndex);
void BG_AddAmmoToClip(playerState_s *ps, int clipIndex, int amount);
int BG_GetAmmoPlayerMax(playerState_s *ps, unsigned int weaponIndex, unsigned int weaponIndexToSkip);
int BG_WeaponIsClipOnly(int weapon);
void PM_WeaponUseAmmo(playerState_s *ps, int wp, int amount);
int PM_WeaponAmmoAvailable(playerState_s *ps);

// /bgame/bg_weapons_ammo.h:42
static int BG_GetClipSize(int weaponIndex)
{
	WeaponVariantDef *weapVariantDef = BG_GetWeaponVariantDef(weaponIndex);

	if (weapVariantDef->iClipSize <= 1)
		return weapVariantDef->iClipSize;

	int clipSize = (int)((float)weapVariantDef->iClipSize * player_clipSizeMultiplier->current.value);

	// Minimum size of one bullet after modifiers
	return max(clipSize, 1);
}

// /bgame/bg_weapons_ammo.h:69
static int BG_GetMaxAmmo(int weapon)
{
	WeaponDef *weap = BG_GetWeaponDef(weapon);

	if (weap->ammoCountClipRelative)
		return weap->iMaxAmmo * BG_GetClipSize(weapon);

	return weap->iMaxAmmo;
}

// /bgame/bg_weapons_ammo.h:118
static int BG_GetAmmoInClipForWeaponDef(playerState_s *ps, WeaponVariantDef *weapVarDef)
{
	ASSERT(ps);
	ASSERT(weapVarDef);

	for (int i = 0; i < ARRAYSIZE(ps->ammoInClip); i++)
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