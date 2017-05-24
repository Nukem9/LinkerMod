#include "stdafx.h"

// /bgame/bg_weapons_def.cpp:62
WeaponVariantDef *BG_GetWeaponVariantDef(unsigned int weaponIndex)
{
	ASSERT_MSG(weaponIndex >= 0 && weaponIndex <= bg_lastParsedWeaponIndex, "weaponIndex not in [0, bg_lastParsedWeaponIndex]");

	return bg_weaponVariantDefs[weaponIndex];
}

// /bgame/bg_weapons_def.cpp:70
WeaponDef *BG_GetWeaponDef(unsigned int weaponIndex)
{
	return BG_GetWeaponVariantDef(weaponIndex)->weapDef;
}

// /bgame/bg_weapons_def.cpp:83
unsigned int BG_GetWeaponIndex(WeaponVariantDef *weapVariantDef)
{
	ASSERT(weapVariantDef);

	for (unsigned int weapIndex = 0; weapIndex <= bg_lastParsedWeaponIndex; weapIndex++)
	{
		if (weapVariantDef == bg_weaponVariantDefs[weapIndex])
			return weapIndex;
	}

	ASSERT_MSG(false, "Weapon variant not in table, unknown weapon index?!");
	return 0;
}

// /bgame/bg_weapons_def.cpp:???
void BG_SetWeaponUsed(int clientIndex, int weapIndex)
{
	((DWORD *)0xBDF508)[weapIndex / 32 + 4 * clientIndex] |= 1 << weapIndex % 32;
}