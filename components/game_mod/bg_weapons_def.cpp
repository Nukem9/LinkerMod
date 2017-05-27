#include "stdafx.h"

std::vector<WeaponVariantDef *> bg_CheckedVariantDefs;
std::vector<WeaponVariantDef *> bg_PatchedVariantDefs;
std::vector<WeaponDef *> bg_PatchedWeapDefs;

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

// /bgame/bg_weapons_def.cpp:274
void BG_ClearWeaponDef()
{
	bg_CheckedVariantDefs.clear();
	bg_PatchedVariantDefs.clear();
	bg_PatchedWeapDefs.clear();

	((void(__cdecl *)())0x004F2900)();
}

// /bgame/bg_weapons_def.cpp:???
void BG_SetWeaponUsed(int clientIndex, int weapIndex)
{
	// Sets a bit flag if a player used the weapon at least once
	((DWORD *)0xBDF508)[weapIndex / 32 + 4 * clientIndex] |= 1 << weapIndex % 32;
}