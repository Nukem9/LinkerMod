#pragma once

enum weapType_t
{
	WEAPTYPE_BULLET = 0x0,
	WEAPTYPE_GRENADE = 0x1,
	WEAPTYPE_PROJECTILE = 0x2,
	WEAPTYPE_BINOCULARS = 0x3,
	WEAPTYPE_GAS = 0x4,
	WEAPTYPE_BOMB = 0x5,
	WEAPTYPE_MINE = 0x6,
	WEAPTYPE_MELEE = 0x7,
	WEAPTYPE_NUM = 0x8,
};

enum weapClass_t
{
	WEAPCLASS_RIFLE = 0x0,
	WEAPCLASS_MG = 0x1,
	WEAPCLASS_SMG = 0x2,
	WEAPCLASS_SPREAD = 0x3,
	WEAPCLASS_PISTOL = 0x4,
	WEAPCLASS_GRENADE = 0x5,
	WEAPCLASS_ROCKETLAUNCHER = 0x6,
	WEAPCLASS_TURRET = 0x7,
	WEAPCLASS_NON_PLAYER = 0x8,
	WEAPCLASS_GAS = 0x9,
	WEAPCLASS_ITEM = 0xA,
	WEAPCLASS_MELEE = 0xB,
	WEAPCLASS_KILLSTREAK_ALT_STORED_WEAPON = 0xC,
	WEAPCLASS_NUM = 0xD,
};

enum PenetrateType
{
	PENETRATE_TYPE_NONE = 0x0,
	PENETRATE_TYPE_SMALL = 0x1,
	PENETRATE_TYPE_MEDIUM = 0x2,
	PENETRATE_TYPE_LARGE = 0x3,
	PENETRATE_TYPE_COUNT = 0x4,
};

enum weapFireType_t
{
	WEAPON_FIRETYPE_FULLAUTO = 0x0,
	WEAPON_FIRETYPE_SINGLESHOT = 0x1,
	WEAPON_FIRETYPE_BURSTFIRE2 = 0x2,
	WEAPON_FIRETYPE_BURSTFIRE3 = 0x3,
	WEAPON_FIRETYPE_BURSTFIRE4 = 0x4,
	WEAPON_FIRETYPE_STACKED = 0x5,
	WEAPON_FIRETYPE_MINIGUN = 0x6,
	WEAPON_FIRETYPECOUNT = 0x7,
};

enum weapInventoryType_t
{
	WEAPINVENTORY_PRIMARY = 0x0,
	WEAPINVENTORY_OFFHAND = 0x1,
	WEAPINVENTORY_ITEM = 0x2,
	WEAPINVENTORY_ALTMODE = 0x3,
	WEAPINVENTORY_MELEE = 0x4,
	WEAPINVENTORYCOUNT = 0x5,
};

struct WeaponDef
{
	char _pad1[0x1C];
	weapType_t weapType;
	weapClass_t weapClass;
	PenetrateType penetrateType;
	char _pad2[0x4];
	weapInventoryType_t inventoryType;
	weapFireType_t fireType;
	char _pad3[0x38];
	int offhandSlot;
	char _pad4[0x2CC];
	int iStartAmmo;
	char _pad5[0x4];
	int iMaxAmmo;
	int shotCount;
	const char *szSharedAmmoCapName;
	int iSharedAmmoCapIndex;
	int iSharedAmmoCap;
	bool unlimitedAmmo;
	bool ammoCountClipRelative;
	char _pad7[0x1F3];
	bool bRifleBullet;
	char _pad8[0x13];
	bool bClipOnly;
	char _pad9[0x8];
	bool bDualWield;
	char _pad10[0x29];
	unsigned int dualWieldWeaponIndex;
	char _pad11[0x95];
	bool bBulletImpactExplode;
	char _pad12[0x10];
	bool holdButtonToThrow;
	char _pad13[0x16D];
	float fMinDamageRange;
};
STATIC_ASSERT_OFFSET(WeaponDef, weapType, 0x1C);
STATIC_ASSERT_OFFSET(WeaponDef, weapClass, 0x20);
STATIC_ASSERT_OFFSET(WeaponDef, penetrateType, 0x24);
STATIC_ASSERT_OFFSET(WeaponDef, inventoryType, 0x2C);
STATIC_ASSERT_OFFSET(WeaponDef, fireType, 0x30);
STATIC_ASSERT_OFFSET(WeaponDef, offhandSlot, 0x6C);
STATIC_ASSERT_OFFSET(WeaponDef, iStartAmmo, 0x33C);
STATIC_ASSERT_OFFSET(WeaponDef, iMaxAmmo, 0x344);
STATIC_ASSERT_OFFSET(WeaponDef, shotCount, 0x348);
STATIC_ASSERT_OFFSET(WeaponDef, szSharedAmmoCapName, 0x34C);
STATIC_ASSERT_OFFSET(WeaponDef, iSharedAmmoCapIndex, 0x350);
STATIC_ASSERT_OFFSET(WeaponDef, iSharedAmmoCap, 0x354);
STATIC_ASSERT_OFFSET(WeaponDef, unlimitedAmmo, 0x358);
STATIC_ASSERT_OFFSET(WeaponDef, ammoCountClipRelative, 0x359);
STATIC_ASSERT_OFFSET(WeaponDef, bRifleBullet, 0x54D);
STATIC_ASSERT_OFFSET(WeaponDef, bClipOnly, 0x561);
STATIC_ASSERT_OFFSET(WeaponDef, bDualWield, 0x56A);
STATIC_ASSERT_OFFSET(WeaponDef, dualWieldWeaponIndex, 0x594);
STATIC_ASSERT_OFFSET(WeaponDef, bBulletImpactExplode, 0x62D);
STATIC_ASSERT_OFFSET(WeaponDef, holdButtonToThrow, 0x63E);
STATIC_ASSERT_OFFSET(WeaponDef, fMinDamageRange, 0x7AC);

struct WeaponVariantDef
{
	const char *szInternalName;
	char _pad1[0x4];
	WeaponDef *weapDef;
	char _pad2[0x10];
	unsigned int altWeaponIndex;
	int iClipSize;
	char _pad3[0x20];
	int iAmmoIndex;
	char _pad4[0x4];
	int iClipIndex;
};
STATIC_ASSERT_OFFSET(WeaponVariantDef, szInternalName, 0x0);
STATIC_ASSERT_OFFSET(WeaponVariantDef, weapDef, 0x8);
STATIC_ASSERT_OFFSET(WeaponVariantDef, altWeaponIndex, 0x1C);
STATIC_ASSERT_OFFSET(WeaponVariantDef, iClipSize, 0x20);
STATIC_ASSERT_OFFSET(WeaponVariantDef, iAmmoIndex, 0x44);
STATIC_ASSERT_OFFSET(WeaponVariantDef, iClipIndex, 0x4C);

struct weaponParms
{
	float forward[3];
	float right[3];
	float up[3];
	float muzzleTrace[3];
	float gunForward[3];
	WeaponVariantDef *weapVariantDef;
	WeaponDef *weapDef;
};
STATIC_ASSERT_OFFSET(weaponParms, forward, 0x0);
STATIC_ASSERT_OFFSET(weaponParms, muzzleTrace, 0x24);
STATIC_ASSERT_OFFSET(weaponParms, weapVariantDef, 0x3C);
STATIC_ASSERT_OFFSET(weaponParms, weapDef, 0x40);

static WeaponVariantDef **bg_weaponVariantDefs = (WeaponVariantDef **)0x00BE19A8;
static unsigned int& bg_lastParsedWeaponIndex = *(unsigned int *)0x00BE1FB0;

extern std::vector<WeaponVariantDef *> bg_CheckedVariantDefs;
extern std::vector<WeaponVariantDef *> bg_PatchedVariantDefs;
extern std::vector<WeaponDef *> bg_PatchedWeapDefs;

WeaponVariantDef *BG_GetWeaponVariantDef(unsigned int weaponIndex);
WeaponDef *BG_GetWeaponDef(unsigned int weaponIndex);
unsigned int BG_GetWeaponIndex(WeaponVariantDef *weapVariantDef);
void BG_ClearWeaponDef();
void BG_SetWeaponUsed(int clientIndex, int weapIndex);