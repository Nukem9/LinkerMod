#include "stdafx.h"

void(*PM_WeaponUseAmmo)(int ps, int wp, int amount);

void hk_PM_WeaponUseAmmo(int ps, int wp, int amount)
{
	if (!player_sustainAmmo->current.enabled)
		PM_WeaponUseAmmo(ps, wp, amount);
}