#include "stdafx.h"

WeaponVariantDef *bg_weaponVariantDefs[2048];
WeaponVariantDefHash bg_weaponVariantNameHashTable[2048];

std::vector<WeaponVariantDef *> bg_CheckedVariantDefs;
std::vector<WeaponVariantDef *> bg_PatchedVariantDefs;
std::vector<WeaponDef *> bg_PatchedWeapDefs;

// /bgame/bg_weapons_def.cpp:57
unsigned int BG_GetNumWeapons()
{
	return bg_lastParsedWeaponIndex + 1;
}

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

// /bgame/bg_weapons_def.cpp:100
void BG_FreeWeaponDefStrings()
{
	for (unsigned int i = 1; i <= bg_lastParsedWeaponIndex; i++)
	{
		WeaponVariantDef *weapVariantDef = bg_weaponVariantDefs[i];
		WeaponDef *weapDef = weapVariantDef->weapDef;
		
		ASSERT(weapVariantDef && weapDef);

		// Weapon hide tag names
		for (int j = 0; j < 32; j++)
		{
			if (weapVariantDef->hideTags[j])
				SL_RemoveRefToString(SCRIPTINSTANCE_SERVER, weapVariantDef->hideTags[j]);
		}

		// Weapon notetrack names
		for (int j = 0; j < 20; j++)
		{
			if (weapDef->notetrackSoundMapKeys[j])
				SL_RemoveRefToString(SCRIPTINSTANCE_SERVER, weapDef->notetrackSoundMapKeys[j]);

			if (weapDef->notetrackSoundMapValues[j])
				SL_RemoveRefToString(SCRIPTINSTANCE_SERVER, weapDef->notetrackSoundMapValues[j]);
		}
	}
}

// /bgame/bg_weapons_def.cpp:159
void BG_FillInWeaponItems(unsigned int weapIndex)
{
	int result = 4 * weapIndex + 0xBDD2B8;
	int v2 = 16;
	do
	{
		*(DWORD *)result = 1;
		result += 512;
		--v2;
	} while (v2);
}

// /bgame/bg_weapons_def.cpp:179
void BG_SetupWeaponAlts(unsigned int weapIndex, void(__cdecl *regWeap)(unsigned int))
{
	ASSERT_MSG(weapIndex >= 0 && weapIndex <= bg_lastParsedWeaponIndex, "weapIndex not in [0, bg_lastParsedWeaponIndex]");

	WeaponVariantDef *weapVariantDef = bg_weaponVariantDefs[weapIndex];
	weapVariantDef->altWeaponIndex = WP_NONE;

	if (*weapVariantDef->szAltWeaponName)
	{
		unsigned int altWeaponIndex = BG_GetWeaponIndexForName(weapVariantDef->szAltWeaponName, regWeap);

		if (!altWeaponIndex)
			Com_Error(ERR_DROP, "Could not find altWeapon '%s' for weapon '%s'", weapVariantDef->szAltWeaponName, BG_WeaponName(weapIndex));

		if (Com_SessionMode_IsZombiesGame() || weapVariantDef->weapDef->inventoryType != WEAPINVENTORY_ALTMODE)
			weapVariantDef->altWeaponIndex = altWeaponIndex;
		else
			weapVariantDef->altWeaponIndex = WP_NONE;
	}
}

// /bgame/bg_weapons_def.cpp:208
void BG_SetupDualWieldAlts(unsigned int weapIndex, void(__cdecl *regWeap)(unsigned int))
{
	WeaponDef *weapDef = bg_weaponVariantDefs[weapIndex]->weapDef;

	// Don't care if not dual wield
	if (!weapDef->bDualWield)
		return;

	weapDef->dualWieldWeaponIndex = WP_NONE;

	if (*weapDef->szDualWieldWeaponName)
	{
		unsigned int altWeaponIndex = BG_GetWeaponIndexForName(weapDef->szDualWieldWeaponName, regWeap);

		if (!altWeaponIndex)
			Com_Error(ERR_DROP, "Could not find alt Dual Wield Weapon '%s' for weapon '%s'",weapDef->szDualWieldWeaponName, BG_WeaponName(weapIndex));

		weapDef->dualWieldWeaponIndex = altWeaponIndex;
	}
}

// /bgame/bg_weapons_def.cpp:231
void BG_SetupWeaponMountedVersions(unsigned int weaponIndex, void(__cdecl *regWeap)(unsigned int))
{
	WeaponDef *weapDef = bg_weaponVariantDefs[weaponIndex]->weapDef;

	weapDef->standMountedIndex = WP_NONE;
	weapDef->crouchMountedIndex = WP_NONE;
	weapDef->proneMountedIndex = WP_NONE;

	if (*weapDef->standMountedWeapdef)
	{
		unsigned int mountedWeaponIndex = BG_GetWeaponIndexForName(weapDef->standMountedWeapdef, regWeap);

		if (!mountedWeaponIndex)
			Com_Error(ERR_DROP, "could not find standingWeapdef '%s' for weapon '%s' Please add the line 'weapon,sp/%s' to your level csv",
				weapDef->standMountedWeapdef,
				BG_WeaponName(weaponIndex),
				weapDef->standMountedWeapdef);

		weapDef->standMountedIndex = mountedWeaponIndex;
	}
	
	if (*weapDef->crouchMountedWeapdef)
	{
		unsigned int mountedWeaponIndex = BG_GetWeaponIndexForName(weapDef->crouchMountedWeapdef, regWeap);

		if (!mountedWeaponIndex)
			Com_Error(ERR_DROP, "could not find crouchingWeapdef '%s' for weapon '%s' Please add the line 'weapon,sp/%s' to your level csv",
				weapDef->crouchMountedWeapdef,
				BG_WeaponName(weaponIndex),
				weapDef->crouchMountedWeapdef);

		weapDef->crouchMountedIndex = mountedWeaponIndex;
	}

	if (*weapDef->proneMountedWeapdef)
	{
		unsigned int mountedWeaponIndex = BG_GetWeaponIndexForName(weapDef->proneMountedWeapdef, regWeap);

		if (!mountedWeaponIndex)
			Com_Error(ERR_DROP, "could not find proneWeapdef '%s' for weapon '%s' Please add the line 'weapon,sp/%s' to your level csv",
				weapDef->proneMountedWeapdef,
				BG_WeaponName(weaponIndex),
				weapDef->proneMountedWeapdef);

		weapDef->proneMountedIndex = mountedWeaponIndex;
	}
}

// /bgame/bg_weapons_def.cpp:265
void BG_InitDefaultWeaponDef()
{
	ASSERT(bg_lastParsedWeaponIndex == 0);

	WeaponVariantDef *defaultWeap = BG_LoadDefaultWeaponVariantDef();

	bg_weaponVariantDefs[0] = defaultWeap;
	bg_weaponVariantNameHashTable[0].weaponIndex = WP_NONE;
	bg_weaponVariantNameHashTable[0].hash = StringTable_HashString(defaultWeap->szInternalName);
	bg_weaponVariantNameHashTableSorted = false;
}

// /bgame/bg_weapons_def.cpp:274
void BG_ClearWeaponDef()
{
	// Set changed variables back to default
	for (auto& weapon : bg_PatchedVariantDefs)
	{
		weapon->weapDef->iStartAmmo -= 1;
		weapon->weapDef->iMaxAmmo -= 1;
		weapon->weapDef->iSharedAmmoCap -= 1;
		weapon->iClipSize -= 1;
	}

	bg_CheckedVariantDefs.clear();
	bg_PatchedVariantDefs.clear();
	bg_PatchedWeapDefs.clear();

	BG_InitDefaultWeaponDef();

	//for (int itemIdx = 1; itemIdx < 2048; itemIdx++)
	//	bg_itemlist[itemIdx].giType = 0;

	memset((void *)0xBDD2BC, 0, 0x1FFC);

	BG_ClearWeaponDefAmmo();
	BG_LoadPlayerAnimTypes();
	BG_LoadWeaponMergeSupport();
	BG_ClearWeaponDefInternal();
}

// /bgame/bg_weapons_def.cpp:298
void BG_SetupWeaponIndex(unsigned int weapIndex)
{
	Com_DPrintf(0, "[Idx %04d] Setting up weapon %s...\n", weapIndex, bg_weaponVariantDefs[weapIndex]->szInternalName);

	// Override the default grenade limit (See PERK_STOCKPILE)
	WeaponVariantDef *variant = BG_GetWeaponVariantDef(weapIndex);

	if (variant->weapDef->weapType == WEAPTYPE_GRENADE && variant->weapDef->inventoryType == WEAPINVENTORY_OFFHAND)
	{
		auto& usedDefs = bg_CheckedVariantDefs;

		// Make sure we've never used this variant before
		if (std::find(usedDefs.begin(), usedDefs.end(), variant) == usedDefs.end())
		{
			usedDefs.push_back(variant);

			variant->weapDef->iStartAmmo += 1;
			variant->weapDef->iMaxAmmo += 1;
			variant->weapDef->iSharedAmmoCap += 1;
			variant->iClipSize += 1;

			bg_PatchedVariantDefs.push_back(variant);
			bg_PatchedWeapDefs.push_back(variant->weapDef);
		}
	}

	// Continue with normal setup
	BG_SetUpAmmoForWeapon(weapIndex);
	BG_FillInWeaponItems(weapIndex);
}

// /bgame/bg_weapons_def.cpp:305
int BG_SetupWeaponVariantDef(WeaponVariantDef *weapVariantDef, void(__cdecl *regWeap)(unsigned int))
{
	ASSERT_MSG(bg_lastParsedWeaponIndex < ARRAYSIZE(bg_weaponVariantDefs), "bg_lastParsedWeaponIndex doesn't index ARRAY_COUNT(bg_weaponVariantDefs)");

	unsigned int weapIndex = bg_lastParsedWeaponIndex + 1;
	bg_lastParsedWeaponIndex = weapIndex;

	bg_weaponVariantDefs[weapIndex] = weapVariantDef;
	bg_weaponVariantNameHashTable[weapIndex].weaponIndex = weapIndex;
	bg_weaponVariantNameHashTable[weapIndex].hash = StringTable_HashString(bg_weaponVariantDefs[weapIndex]->szInternalName);
	bg_weaponVariantNameHashTableSorted = false;

	BG_SetupWeaponIndex(weapIndex);
	BG_SetupWeaponAlts(weapIndex, regWeap);
	BG_SetupDualWieldAlts(weapIndex, regWeap);
	BG_SetupWeaponMountedVersions(weapIndex, regWeap);

	if (regWeap)
		regWeap(weapIndex);

	return weapIndex;
}

// /bgame/bg_weapons_def.cpp:335
void BG_FillInAllWeaponItems()
{
	BG_ResetWeaponUsedMasks();

	for (unsigned int weaponIndex = 1; weaponIndex < BG_GetNumWeapons(); weaponIndex++)
		BG_SetupWeaponIndex(weaponIndex);
}

// /bgame/bg_weapons_ammo.cpp:348
void BG_SetupAmmoIndexes(unsigned int weapIndex)
{
	BG_SetupWeaponDefAmmoIndexes(weapIndex, bg_weaponVariantDefs[weapIndex]->weapDef, bg_weaponVariantDefs[weapIndex]);
}

// /bgame/bg_weapons_ammo.cpp:353
void BG_SetupSharedAmmoIndexes(unsigned int weapIndex)
{
	BG_SetupWeaponDefSharedAmmoIndexes(weapIndex, bg_weaponVariantDefs[weapIndex]->weapDef);
}

// /bgame/bg_weapons_ammo.cpp:358
void BG_SetupClipIndexes(unsigned int weapIndex)
{
	BG_SetupWeaponDefClipIndexes(bg_weaponVariantDefs[weapIndex]->weapDef, bg_weaponVariantDefs[weapIndex]);
}

// /bgame/bg_weapons_def.cpp:363
void BG_SetUpAmmoForWeapon(unsigned int weapIndex)
{
	BG_SetupAmmoIndexes(weapIndex);
	BG_SetupSharedAmmoIndexes(weapIndex);
	BG_SetupClipIndexes(weapIndex);
}

// /bgame/bg_weapons_def.cpp:371
int BG_WeaponVariantNameHashCompare(const void *a, const void *b)
{
	auto weapA = (WeaponVariantDefHash *)a;
	auto weapB = (WeaponVariantDefHash *)b;

	if (weapA->hash <= weapB->hash)
	{
		if (weapA->hash >= weapB->hash)
			return 0;

		return -1;
	}

	return 1;
}

// /bgame/bg_weapons_def.cpp:385
int BG_FindWeaponIndexForName(const char *name)
{
	if (!name)
		return 0;

	// Hash the weapon name before comparing anything
	int nameHash = StringTable_HashString(name);

	if (!bg_weaponVariantNameHashTableSorted)
	{
		std::qsort(bg_weaponVariantNameHashTable, bg_lastParsedWeaponIndex + 1, sizeof(WeaponVariantDefHash), BG_WeaponVariantNameHashCompare);
		bg_weaponVariantNameHashTableSorted = true;
	}

	// Binary search
	WeaponVariantDefHash key;
	key.hash = nameHash;
	key.weaponIndex = 0;

	auto ret = (WeaponVariantDefHash *)std::bsearch(
		&key,
		bg_weaponVariantNameHashTable,
		bg_lastParsedWeaponIndex + 1,
		sizeof(WeaponVariantDefHash),
		BG_WeaponVariantNameHashCompare);

	if (ret)
		return ret->weaponIndex;

	// Weapon name wasn't found
	return 0;
}

// /bgame/bg_weapons_def.cpp:420
bool BG_IsDefaultWeapon(const char *name)
{
	// If a mod is loaded or fastfiles are disabled
	if ((fs_gameDirVar && *fs_gameDirVar->current.string) || !useFastFile->current.enabled)
		return false;

	return DB_IsXAssetDefault(ASSET_TYPE_WEAPON, name);
}

// /bgame/bg_weapons_def.cpp:429
unsigned int BG_GetWeaponIndexForName(const char *name, void(__cdecl *regWeap)(unsigned int))
{
	// The weapon "none" is hardcoded as 0
	if (!*name || !_stricmp(name, "none"))
		return 0;

	// Check if the weapon was loaded already
	unsigned int weapIndex = BG_FindWeaponIndexForName(name);

	if (weapIndex != WP_NONE)
		return weapIndex;

	// Try to load it
	WeaponVariantDef *weapVariantDef = BG_LoadWeaponVariantDef(name);

	if (!weapVariantDef)
	{
		Com_DPrintf(17, "Couldn't find weapon \"%s\"\n", name);
		return 0;
	}

	// The default weapon also maps to index 0
	if (BG_IsDefaultWeapon(name))
		return 0;

	return BG_SetupWeaponVariantDef(weapVariantDef, regWeap);
}

// /bgame/bg_weapons_def.cpp:456
unsigned int BG_GetWeaponIndexForName(const char *name)
{
	return BG_GetWeaponIndexForName(name, nullptr);
}

// /bgame/bg_weapons_def.cpp:???
void BG_ResetWeaponUsedMasks()
{
	memset((void *)0x00BDF508, 0, 0x40);
}

// /bgame/bg_weapons_def.cpp:???
void BG_SetWeaponUsed(int clientIndex, int weapIndex)
{
	// Sets a bit flag if a player used the weapon at least once
	((DWORD *)0xBDF508)[weapIndex / 32 + 4 * clientIndex] |= 1 << weapIndex % 32;
}