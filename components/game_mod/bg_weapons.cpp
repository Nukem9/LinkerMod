#include "stdafx.h"

// /bgame/bg_weapons.cpp:278
void PM_StartWeaponAnim(playerState_s *ps, int anim, int leftAnim)
{
	if (ps->pm_type < 9)
	{
		if (leftAnim)
			ps->weapAnimLeft = anim | (~ps->weapAnimLeft & 0x400);
		else
			ps->weapAnim = anim | (~ps->weapAnim & 0x400);
	}
}

// /bgame/bg_weapons.cpp:290
void PM_ContinueWeaponAnim(playerState_s *ps, int anim, int leftAnim)
{
	if (((ps->weapAnim & ~0x400) != anim || ps->bRunLeftGun) &&
		((ps->weapAnimLeft & ~0x400) != anim || !ps->bRunLeftGun))
	{
		PM_StartWeaponAnim(ps, anim, leftAnim);
	}
}

// /bgame/bg_weapons.cpp:432
int *BG_GetWeaponState(playerState_s *ps, bool leftGun)
{
	if (leftGun)
		return &ps->weaponstateLeft;

	return &ps->weaponstate;
}

// /bgame/bg_weapons.cpp:440
int *BG_GetWeaponTime(playerState_s *ps, bool leftGun)
{
	if (leftGun)
		return &ps->weaponTimeLeft;

	return &ps->weaponTime;
}

// /bgame/bg_weapons.cpp:448
int *BG_GetWeaponDelay(playerState_s *ps, bool leftGun)
{
	if (leftGun)
		return &ps->weaponDelayLeft;
	
	return &ps->weaponDelay;
}

// /bgame/bg_weapons.cpp:456
unsigned int *BG_GetWeaponShotCount(playerState_s *ps, bool leftGun)
{
	if (leftGun)
		return &ps->weaponShotCountLeft;

	return &ps->weaponShotCount;
}

// /bgame/bg_weapons.cpp:1204
void PM_Weapon_FinishRechamber(playerState_s *ps)
{
	PM_ContinueWeaponAnim(ps, 0, 0);

	PlayerHeldWeapon *weap = BG_GetHeldWeapon(ps, ps->weapon);

	if (weap)
		weap->needsRechamber = false;

	ps->weaponstate = WEAPON_READY;
}

// /bgame/bg_weapons.cpp:1266
unsigned int PM_GetWeaponIndexForHand(playerState_s *ps)
{
	if (ps->bRunLeftGun)
		return BG_GetWeaponDef(ps->weapon)->dualWieldWeaponIndex;

	return ps->weapon;
}

// /bgame/bg_weapons.cpp:1518
bool BG_CanFastSwitch(WeaponDef *weapDef, int weaponState)
{
	switch (weaponState)
	{
	case WEAPON_RAISING:
	case WEAPON_RAISING_ALTSWITCH:
	case WEAPON_DROPPING:
	case WEAPON_DROPPING_QUICK:
	case WEAPON_DROPPING_ALTSWITCH:
		return weapDef->offhandSlot != 4;
	}

	return false;
}

// /bgame/bg_weapons.cpp:2541
bool ShotLimitReached(playerState_s *ps, WeaponDef *weapDef)
{
	ASSERT(ps);
	ASSERT(weapDef);

	unsigned int *weaponShotCount = BG_GetWeaponShotCount(ps, ps->bRunLeftGun);

	switch (weapDef->fireType)
	{
	case WEAPON_FIRETYPE_SINGLESHOT:return *weaponShotCount >= 1;
	case WEAPON_FIRETYPE_BURSTFIRE2:return *weaponShotCount >= 2;
	case WEAPON_FIRETYPE_BURSTFIRE3:return *weaponShotCount >= 3;
	case WEAPON_FIRETYPE_BURSTFIRE4:return *weaponShotCount >= 4;
	case WEAPON_FIRETYPE_STACKED:	return *weaponShotCount >= ps->stackFireCount;
	}

	return false;
}

// /bgame/bg_weapons.cpp:2580
bool BurstFirePending(playerState_s *ps)
{
	static DWORD dwCall = 0x00766800;

	__asm
	{
		mov esi, ps
		call dwCall
	}
}

// /bgame/bg_weapons.cpp:2614
bool WeaponUsesBurstCooldown(unsigned int weaponIdx)
{
	if (weaponIdx)
	{
		switch (BG_GetWeaponDef(weaponIdx)->fireType)
		{
		case WEAPON_FIRETYPE_BURSTFIRE2:
		case WEAPON_FIRETYPE_BURSTFIRE3:
		case WEAPON_FIRETYPE_BURSTFIRE4:
			return true;
		}
	}

	return false;
}

// /bgame/bg_weapons.cpp:2699
int PM_Weapon_WeaponTimeAdjust(pmove_t *pm, pml_t *pml)
{
	playerState_s *ps = pm->ps;
	ASSERT(ps);

	int msec = 0;
	int *weaponState = BG_GetWeaponState(ps, ps->bRunLeftGun);
	int *weaponTime = BG_GetWeaponTime(ps, ps->bRunLeftGun);
	int *weaponDelay = BG_GetWeaponDelay(ps, ps->bRunLeftGun);
	unsigned int *weaponShotCount = BG_GetWeaponShotCount(ps, ps->bRunLeftGun);
	unsigned int weaponIndex = PM_GetWeaponIndexForHand(ps);

	WeaponVariantDef *weapVariantDef = BG_GetWeaponVariantDef(weaponIndex);
	WeaponDef *weapDef = BG_GetWeaponDef(weaponIndex);

	if (ps->weaponRestrictKickTime > 0)
		ps->weaponRestrictKickTime = max(ps->weaponRestrictKickTime - pml->msec, 0);

	// Apply perk modifications to the animation time
	if (IS_WEAPONSTATE_RELOAD(*weaponState) && BG_HasPerk(ps->perks, PERK_FASTRELOAD))
	{
		// Fast weapon reload perk
		if (perk_weapReloadMultiplier->current.value != 0.0f)
			msec = (int)((float)pml->msec / perk_weapReloadMultiplier->current.value);
		else
			msec = max(*weaponDelay, *weaponTime);
	}
	else if (IS_WEAPONSTATE_FIRE(*weaponState) && !IS_WEAPONSTATE_MELEE(*weaponState) && BG_HasPerk(ps->perks, PERK_RATEOFFIRE))
	{
		// Doubletap (rate of fire) perk
		if (perk_weapRateMultiplier->current.value != 0.0f)
			msec = (int)((float)pml->msec / perk_weapRateMultiplier->current.value);
		else
			msec = max(*weaponDelay, *weaponTime);
	}
	else if (BG_CanFastSwitch(weapDef, *weaponState) && BG_HasPerk(ps->perks, PERK_FASTSWITCH))
	{
		// Fast weapon switch perk
		ASSERT(perk_weapSwitchMultiplier->current.value > 0.0f);

		msec = (int)((float)pml->msec / perk_weapSwitchMultiplier->current.value);
	}
	else
	{
		// Default modifier time
		msec = pml->msec;
	}

	if (*weaponTime)
	{
		*weaponTime -= msec;

		if (weapDef->fireType == WEAPON_FIRETYPE_STACKED && BurstFirePending(ps))
			*weaponTime = 0;

		if (*weaponTime <= 0)
		{
			if ((*weaponState == WEAPON_FIRING || *weaponState == WEAPON_CONT_FIRE_LOOP) &&
				WeaponUsesBurstCooldown(weaponIndex) && !BurstFirePending(ps))
			{
				float scalar = player_burstFireCooldown->current.value;

				if (scalar != 0.0f)
					*weaponTime = (int)(scalar * 1000.0f);
				else
					*weaponTime = 1;

				// Bugfix? BO2 uses ps->bRunLeftGun
				PM_ContinueWeaponAnim(ps, 0, ps->bRunLeftGun);
				*weaponState = WEAPON_READY;
				return 0;
			}

			bool pausedAfterFiring = !(ps->weapFlags & 0x400) && ShotLimitReached(ps, weapDef);
			bool holdingGrenadeBtn = (weapDef->weapType == WEAPTYPE_GRENADE || weapDef->weapType == WEAPTYPE_MINE) && weapDef->holdButtonToThrow;
			bool holdingFireBtn = false;

			if (ps->bRunLeftGun)
				holdingFireBtn = pm->cmd.button_bits.testBit(0x18) != 0;
			else
				holdingFireBtn = pm->cmd.button_bits.testBit(0) != 0;

			if (!IS_WEAPONSTATE_OFFHAND(*weaponState)
				&& *weaponState != WEAPON_DROPPING_QUICK
				&& *weaponState != WEAPON_RAISING
				&& (pausedAfterFiring || holdingGrenadeBtn)
				&& holdingFireBtn
				&& ps->weapon == pm->cmd.weapon
				&& (PM_WeaponAmmoAvailable(ps) || weapDef->unlimitedAmmo))
			{
				*weaponTime = 1;

				if (IS_WEAPONSTATE_RELOAD(*weaponState))
				{
					*weaponTime = 0;
					*weaponShotCount = 0;
				}
				else if (*weaponState == WEAPON_RECHAMBERING)
				{
					PM_Weapon_FinishRechamber(ps);
				}
				else if (IS_WEAPONSTATE_FIRE(*weaponState))
				{
					// Bugfix? BO2 uses ps->bRunLeftGun
					PM_ContinueWeaponAnim(ps, 0, ps->bRunLeftGun);
					*weaponState = WEAPON_READY;
				}
			}
			else
			{
				if ((!holdingFireBtn || ps->weapFlags & 0x400 || *weaponState == WEAPON_DROPPING_QUICK || *weaponState == WEAPON_RAISING) && !BurstFirePending(ps))
					*weaponShotCount = 0;

				*weaponTime = 0;
			}
		}
	}

	if (!*weaponDelay)
		return 0;

	*weaponDelay -= msec;

	if (*weaponDelay > 0)
		return 0;

	*weaponDelay = 0;
	return 1;
}

// /bgame/bg_weapons.cpp:2985
int PM_GetWeaponFireButton(unsigned int weapon)
{
	WeaponDef *weapDef = BG_GetWeaponDef(weapon);

	ASSERT(weapDef);

	if ((weapDef->weapType == WEAPTYPE_GRENADE || weapDef->weapType == WEAPTYPE_MINE) && weapDef->hasDetonator)
		return 24;

	return 0;
}

// /bgame/bg_weapons.cpp:2998
int PM_Weapon_ShouldBeFiring(pmove_t *pm, int delayedAction, bool testOnly)
{
	playerState_s *ps = pm->ps;
	ASSERT(ps);

	if (ps->pm_flags & 0x200000)
		return 0;

	WeaponDef *weapDef = BG_GetWeaponDef(ps->weapon);
	int weaponBit = PM_GetWeaponFireButton(ps->weapon);
	bool shouldStartFiring = pm->cmd.button_bits.testBit(weaponBit) != 0;

	// If this is a dw weapon and we're using the offhand
	if (ps->bRunLeftGun && weapDef->bDualWield)
	{
		// Check if player holding the fire key
		if (pm->cmd.button_bits.testBit(24))
			shouldStartFiring = true;
		else
			shouldStartFiring = false;
	}

	if (weapDef->freezeMovementWhenFiring && ps->groundEntityNum == 1023)
		shouldStartFiring = false;

	if (weapDef->bCanUseInVehicle)
		shouldStartFiring = false;

	if (weapDef->fireType == WEAPON_FIRETYPE_MINIGUN && shouldStartFiring)
		shouldStartFiring = ps->weaponSpinLerp >= 1.0f;

	if (shouldStartFiring || (delayedAction || BurstFirePending(ps)))
		return 1;

	if (!testOnly)
	{
		if (ps->bRunLeftGun && weapDef->bDualWield)
		{
			if (ps->weaponstateLeft == WEAPON_FIRING)
				PM_ContinueWeaponAnim(ps, 0, 1);

			ps->weaponstateLeft = WEAPON_READY;
		}
		else
		{
			if (ps->weaponstate == WEAPON_FIRING && !Mantle_IsWeaponEquipped(ps))
				PM_ContinueWeaponAnim(ps, 0, 0);

			ps->weaponstate = WEAPON_READY;
		}
	}

	return 0;
}

void __declspec(naked) hk_PM_Weapon_ShouldBeFiring()
{
	__asm
	{
		push [esp + 8]
		push [esp + 8]
		push eax
		call PM_Weapon_ShouldBeFiring
		add esp, 0xC
		retn
	}
}

// /bgame/bg_weapons.cpp:3276
void PM_Weapon_Jam(/*playerState_s *ps*/)
{
	// Do nothing. ESI is argument.
}

// /bgame/bg_weapons.cpp:5958
WeaponVariantDef *BG_LoadWeaponVariantDef(const char *name)
{
	// Load from rawfiles: BG_LoadWeaponVariantDef_LoadObj
	if (!useFastFile->current.enabled)
		return ((WeaponVariantDef *(__cdecl *)(const char *))0x00768110)(name);

	// Load from fastfile: BG_LoadWeaponVariantDef_FastFile
	return ((WeaponVariantDef *(__cdecl *)(const char *))0x00768170)(name);
}

// /bgame/bg_weapons.cpp:6072
const char *BG_WeaponName(int weapon)
{
	return BG_GetWeaponVariantDef(weapon)->szInternalName;
}

bool Mantle_IsWeaponEquipped(playerState_s *ps)
{
	return ((bool (__cdecl *)(playerState_s *))0x0058D3A0)(ps);
}