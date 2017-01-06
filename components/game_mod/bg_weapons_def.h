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

typedef int WeaponVariantDef;

struct WeaponDef
{
	char _pad1[0x1C];
	weapType_t weapType;
	weapClass_t weapClass;
	PenetrateType penetrateType;
	char _pad2[0x320];
	int shotCount;
	char _pad3[0x201];
	bool bRifleBullet;
	char _pad4[0xDF];
	bool bBulletImpactExplode;
	char _pad5[0x17D];
	float fMinDamageRange;
};
STATIC_ASSERT_OFFSET(WeaponDef, weapType, 0x1C);
STATIC_ASSERT_OFFSET(WeaponDef, weapClass, 0x20);
STATIC_ASSERT_OFFSET(WeaponDef, penetrateType, 0x24);
STATIC_ASSERT_OFFSET(WeaponDef, shotCount, 0x348);
STATIC_ASSERT_OFFSET(WeaponDef, bRifleBullet, 0x54D);
STATIC_ASSERT_OFFSET(WeaponDef, bBulletImpactExplode, 0x62D);
STATIC_ASSERT_OFFSET(WeaponDef, fMinDamageRange, 0x7AC);

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

unsigned int BG_GetWeaponIndex(WeaponVariantDef *weapVariantDef);
void BG_SetWeaponUsed(int clientIndex, int weapIndex);