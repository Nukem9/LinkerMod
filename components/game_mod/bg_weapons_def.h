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
	char _pad0[0x10];
	unsigned __int16 *notetrackSoundMapKeys;
	unsigned __int16 *notetrackSoundMapValues;
	char _pad1[0x4];
	weapType_t weapType;
	weapClass_t weapClass;
	PenetrateType penetrateType;
	char _pad2[0x4];
	weapInventoryType_t inventoryType;
	weapFireType_t fireType;
	char _pad3[0x38];
	int offhandSlot;
	char _pad4[0x108];
	const char *standMountedWeapdef;
	const char *crouchMountedWeapdef;
	const char *proneMountedWeapdef;
	int standMountedIndex;
	int crouchMountedIndex;
	int proneMountedIndex;
	char _pad5[0x17C];
	struct XModel **worldModel;
	char _pad6[0xC];
	struct XModel *additionalMeleeModel;
	char _pad7[0x1C];
	int iStartAmmo;
	char _pad8[0x4];
	int iMaxAmmo;
	int shotCount;
	const char *szSharedAmmoCapName;
	int iSharedAmmoCapIndex;
	int iSharedAmmoCap;
	bool unlimitedAmmo;
	bool ammoCountClipRelative;
	char _pad9[0x1F2];
	bool sharedAmmo;
	bool bRifleBullet;
	char _pad10[0x13];
	bool bClipOnly;
	char _pad11[0x8];
	bool bDualWield;
	char _pad12[0x25];
	const char *szDualWieldWeaponName;
	unsigned int dualWieldWeaponIndex;
	char _pad13[0x50];
	struct XModel *projectileModel;
	char _pad14[0x41];
	bool bBulletImpactExplode;
	char _pad15[0x10];
	bool holdButtonToThrow;
	char _pad16[0x145];
	const char *szUseHintString;
	const char *dropHintString;
	int iUseHintStringIndex;
	int dropHintStringIndex;
	char _pad17[0x18];
	float fMinDamageRange;
};
STATIC_ASSERT_OFFSET(WeaponDef, notetrackSoundMapKeys, 0x10);
STATIC_ASSERT_OFFSET(WeaponDef, notetrackSoundMapValues, 0x14);
STATIC_ASSERT_OFFSET(WeaponDef, weapType, 0x1C);
STATIC_ASSERT_OFFSET(WeaponDef, weapClass, 0x20);
STATIC_ASSERT_OFFSET(WeaponDef, penetrateType, 0x24);
STATIC_ASSERT_OFFSET(WeaponDef, inventoryType, 0x2C);
STATIC_ASSERT_OFFSET(WeaponDef, fireType, 0x30);
STATIC_ASSERT_OFFSET(WeaponDef, offhandSlot, 0x6C);
STATIC_ASSERT_OFFSET(WeaponDef, standMountedWeapdef, 0x178);
STATIC_ASSERT_OFFSET(WeaponDef, crouchMountedWeapdef, 0x17C);
STATIC_ASSERT_OFFSET(WeaponDef, proneMountedWeapdef, 0x180);
STATIC_ASSERT_OFFSET(WeaponDef, standMountedIndex, 0x184);
STATIC_ASSERT_OFFSET(WeaponDef, crouchMountedIndex, 0x188);
STATIC_ASSERT_OFFSET(WeaponDef, proneMountedIndex, 0x18C);
STATIC_ASSERT_OFFSET(WeaponDef, worldModel, 0x30C);
STATIC_ASSERT_OFFSET(WeaponDef, additionalMeleeModel, 0x31C);
STATIC_ASSERT_OFFSET(WeaponDef, iStartAmmo, 0x33C);
STATIC_ASSERT_OFFSET(WeaponDef, iMaxAmmo, 0x344);
STATIC_ASSERT_OFFSET(WeaponDef, shotCount, 0x348);
STATIC_ASSERT_OFFSET(WeaponDef, szSharedAmmoCapName, 0x34C);
STATIC_ASSERT_OFFSET(WeaponDef, iSharedAmmoCapIndex, 0x350);
STATIC_ASSERT_OFFSET(WeaponDef, iSharedAmmoCap, 0x354);
STATIC_ASSERT_OFFSET(WeaponDef, unlimitedAmmo, 0x358);
STATIC_ASSERT_OFFSET(WeaponDef, ammoCountClipRelative, 0x359);
STATIC_ASSERT_OFFSET(WeaponDef, sharedAmmo, 0x54C);
STATIC_ASSERT_OFFSET(WeaponDef, bRifleBullet, 0x54D);
STATIC_ASSERT_OFFSET(WeaponDef, bClipOnly, 0x561);
STATIC_ASSERT_OFFSET(WeaponDef, bDualWield, 0x56A);
STATIC_ASSERT_OFFSET(WeaponDef, szDualWieldWeaponName, 0x590);
STATIC_ASSERT_OFFSET(WeaponDef, dualWieldWeaponIndex, 0x594);
STATIC_ASSERT_OFFSET(WeaponDef, projectileModel, 0x5E8);
STATIC_ASSERT_OFFSET(WeaponDef, bBulletImpactExplode, 0x62D);
STATIC_ASSERT_OFFSET(WeaponDef, holdButtonToThrow, 0x63E);
STATIC_ASSERT_OFFSET(WeaponDef, szUseHintString, 0x784);
STATIC_ASSERT_OFFSET(WeaponDef, dropHintString, 0x788);
STATIC_ASSERT_OFFSET(WeaponDef, iUseHintStringIndex, 0x78C);
STATIC_ASSERT_OFFSET(WeaponDef, dropHintStringIndex, 0x790);
STATIC_ASSERT_OFFSET(WeaponDef, fMinDamageRange, 0x7AC);

struct WeaponVariantDef
{
	const char *szInternalName;
	char _pad1[0x4];
	WeaponDef *weapDef;
	char _pad2[0x8];
	const char *szAltWeaponName;
	unsigned __int16 *hideTags;
	unsigned int altWeaponIndex;
	int iClipSize;
	char _pad4[0x1C];
	const char *szAmmoName;
	int iAmmoIndex;
	const char *szClipName;
	int iClipIndex;
};
STATIC_ASSERT_OFFSET(WeaponVariantDef, szInternalName, 0x0);
STATIC_ASSERT_OFFSET(WeaponVariantDef, weapDef, 0x8);
STATIC_ASSERT_OFFSET(WeaponVariantDef, szAltWeaponName, 0x14);
STATIC_ASSERT_OFFSET(WeaponVariantDef, hideTags, 0x18);
STATIC_ASSERT_OFFSET(WeaponVariantDef, altWeaponIndex, 0x1C);
STATIC_ASSERT_OFFSET(WeaponVariantDef, iClipSize, 0x20);
STATIC_ASSERT_OFFSET(WeaponVariantDef, szAmmoName, 0x40);
STATIC_ASSERT_OFFSET(WeaponVariantDef, iAmmoIndex, 0x44);
STATIC_ASSERT_OFFSET(WeaponVariantDef, szClipName, 0x48);
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

struct WeaponVariantDefHash
{
	int	hash;
	int	weaponIndex;
};
STATIC_ASSERT_OFFSET(WeaponVariantDefHash, hash, 0x0);
STATIC_ASSERT_OFFSET(WeaponVariantDefHash, weaponIndex, 0x4);

extern WeaponVariantDef *bg_weaponVariantDefs[2048];
extern WeaponVariantDefHash bg_weaponVariantNameHashTable[2048];

static unsigned int& bg_firstWeaponTableIndex = *(unsigned int *)0x00BE1FA8;
static unsigned int& bg_lastWeaponTableIndex = *(unsigned int *)0x00BE1FAC;
static unsigned int& bg_lastParsedWeaponIndex = *(unsigned int *)0x00BE1FB0;
static bool& bg_weaponVariantNameHashTableSorted = *(bool *)0x00BE1FB4;

extern std::vector<WeaponVariantDef *> bg_CheckedVariantDefs;
extern std::vector<WeaponVariantDef *> bg_PatchedVariantDefs;
extern std::vector<WeaponDef *> bg_PatchedWeapDefs;

unsigned int BG_GetNumWeapons();
WeaponVariantDef *BG_GetWeaponVariantDef(unsigned int weaponIndex);
WeaponDef *BG_GetWeaponDef(unsigned int weaponIndex);
unsigned int BG_GetWeaponIndex(WeaponVariantDef *weapVariantDef);
void BG_FreeWeaponDefStrings();
void BG_FillInWeaponItems(unsigned int weapIndex);
void BG_SetupWeaponAlts(unsigned int weapIndex, void(__cdecl *regWeap)(unsigned int));
void BG_SetupDualWieldAlts(unsigned int weapIndex, void(__cdecl *regWeap)(unsigned int));
void BG_SetupWeaponMountedVersions(unsigned int weaponIndex, void(__cdecl *regWeap)(unsigned int));
void BG_InitDefaultWeaponDef();
void BG_ClearWeaponDef();
void BG_SetupWeaponIndex(unsigned int weapIndex);
int BG_SetupWeaponVariantDef(WeaponVariantDef *weapVariantDef, void(__cdecl *regWeap)(unsigned int));
void BG_FillInAllWeaponItems();
void BG_SetupAmmoIndexes(unsigned int weapIndex);
void BG_SetupSharedAmmoIndexes(unsigned int weapIndex);
void BG_SetupClipIndexes(unsigned int weapIndex);
void BG_SetUpAmmoForWeapon(unsigned int weapIndex);
int BG_WeaponVariantNameHashCompare(const void *a, const void *b);
int BG_FindWeaponIndexForName(const char *name);
bool BG_IsDefaultWeapon(const char *name);
unsigned int BG_GetWeaponIndexForName(const char *name, void(__cdecl *regWeap)(unsigned int));
unsigned int BG_GetWeaponIndexForName(const char *name);
void BG_ResetWeaponUsedMasks();
void BG_SetWeaponUsed(int clientIndex, int weapIndex);