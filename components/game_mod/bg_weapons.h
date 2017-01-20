#pragma once

enum weaponstate_t
{
	WEAPON_READY = 0x0,
	WEAPON_RAISING = 0x1,
	WEAPON_RAISING_ALTSWITCH = 0x2,
	WEAPON_DROPPING = 0x3,
	WEAPON_DROPPING_QUICK = 0x4,
	WEAPON_DROPPING_ALTSWITCH = 0x5,
	WEAPON_FIRING = 0x6,
	WEAPON_RECHAMBERING = 0x7,
	// probably an extra field here 0x8 WEAPON_RECHAMBER_START
	WEAPON_RELOADING_RIGHT = 0x8,
	WEAPON_RELOADING_LEFT = 0x9,
	WEAPON_RELOADING = 0xA,
	WEAPON_RELOADING_INTERUPT = 0xB,
	WEAPON_RELOAD_START = 0xC,
	WEAPON_RELOAD_START_INTERUPT = 0xD,
	WEAPON_RELOAD_END = 0xE,
	WEAPON_RELOAD_QUICK = 0xF,
	WEAPON_RELOAD_QUICK_EMPTY = 0x10,
	WEAPON_MELEE_INIT = 0x11,
	WEAPON_MELEE_FIRE = 0x12,
	WEAPON_MELEE_END = 0x13,
	WEAPON_OFFHAND_INIT = 0x14,
	WEAPON_OFFHAND_PREPARE = 0x15,
	WEAPON_OFFHAND_HOLD = 0x16,
	WEAPON_OFFHAND_START = 0x17,
	WEAPON_OFFHAND = 0x18,
	WEAPON_OFFHAND_END = 0x19,
	WEAPON_DETONATING = 0x1A,
	WEAPON_SPRINT_RAISE = 0x1B,
	WEAPON_SPRINT_LOOP = 0x1C,
	WEAPON_SPRINT_DROP = 0x1D,
	WEAPON_CONT_FIRE_IN = 0x1E,
	WEAPON_CONT_FIRE_LOOP = 0x1F,
	WEAPON_CONT_FIRE_OUT = 0x20,
	WEAPON_NIGHTVISION_WEAR = 0x21,
	WEAPON_NIGHTVISION_REMOVE = 0x22,
	WEAPON_DEPLOYING = 0x23,
	WEAPON_DEPLOYED = 0x24,
	WEAPON_BREAKING_DOWN = 0x25,
	WEAPON_SWIM_IN = 0x26,
	WEAPON_SWIM_OUT = 0x27,
	WEAPON_DTP_IN = 0x28,
	WEAPON_DTP_LOOP = 0x29,
	WEAPON_DTP_OUT = 0x2A,
	WEAPON_SLIDE_IN = 0x2B,
	WEAPON_FIRING_LEFT = 0x2C,
	WEAPON_FIRING_BOTH = 0x2D,
	WEAPON_JAMMED = 0x2E,
	WEAPON_LOWREADY_RAISE = 0x2F,
	WEAPON_LOWREADY_LOOP = 0x30,
	WEAPON_LOWREADY_DROP = 0x31,
	WEAPONSTATES_NUM = 0x32,
};

#define IS_WEAPONSTATE_RELOAD(x) (\
								(x) == 0xB || \
								(x) == 0xC || \
								(x) == 0xD || \
								(x) == 0xE || \
								(x) == 0xF || \
								(x) == 0x10 || \
								(x) == 0x11)

#define IS_WEAPONSTATE_MELEE(x) (\
								(x) == 0x12 || \
								(x) == 0x13 || \
								(x) == 0x14)

#define IS_WEAPONSTATE_FIRE(x) (\
								(x) == 0x6 || \
								(x) == 0x7 || \
								(x) == 0x8 || \
								(x) == 0x20 || \
								IS_WEAPONSTATE_MELEE(x))

struct playerState_s
{
	int commandTime;
	int pm_type;
	char _pad1[0x8];
	int weapFlags;
	char _pad2[0x28];
	int weaponTime;
	int weaponDelay;
	int weaponTimeLeft;
	int weaponDelayLeft;
	char _pad3[0x10];
	int weaponRestrictKickTime;
	char _pad4[0x1];
	bool bRunLeftGun;
	char _pad5[0xE2];
	unsigned __int16 weapon;
	char _pad6[0x11];
	int weaponstate;
	int weaponstateLeft;
	unsigned int weaponShotCount;
	unsigned int weaponShotCountLeft;
	char _pad7[0x48];
	unsigned int stackFireCount;
	char _pad8[0x348];
	unsigned int perks[1];
	char _pad9[0x24];
	int weapAnim;
	int weapAnimLeft;
};
STATIC_ASSERT_OFFSET(playerState_s, commandTime, 0x0);
STATIC_ASSERT_OFFSET(playerState_s, pm_type, 0x4);
STATIC_ASSERT_OFFSET(playerState_s, weapFlags, 0x10);
STATIC_ASSERT_OFFSET(playerState_s, weaponTime, 0x3C);
STATIC_ASSERT_OFFSET(playerState_s, weaponDelay, 0x40);
STATIC_ASSERT_OFFSET(playerState_s, weaponTimeLeft, 0x44);
STATIC_ASSERT_OFFSET(playerState_s, weaponDelayLeft, 0x48);
STATIC_ASSERT_OFFSET(playerState_s, weaponRestrictKickTime, 0x5C);
STATIC_ASSERT_OFFSET(playerState_s, bRunLeftGun, 0x61);
STATIC_ASSERT_OFFSET(playerState_s, weapon, 0x144);
STATIC_ASSERT_OFFSET(playerState_s, weaponstate, 0x158);
STATIC_ASSERT_OFFSET(playerState_s, weaponstateLeft, 0x15C);
STATIC_ASSERT_OFFSET(playerState_s, weaponShotCount, 0x160);
STATIC_ASSERT_OFFSET(playerState_s, weaponShotCountLeft, 0x164);
STATIC_ASSERT_OFFSET(playerState_s, stackFireCount, 0x1B0);
STATIC_ASSERT_OFFSET(playerState_s, perks, 0x4FC);
STATIC_ASSERT_OFFSET(playerState_s, weapAnim, 0x524);
STATIC_ASSERT_OFFSET(playerState_s, weapAnimLeft, 0x528);

struct usercmd_s
{
	int serverTime;
	bitarray<51> buttons;
	int angles[3];
	unsigned __int16 weapon;

	// The rest omitted
};
STATIC_ASSERT_OFFSET(usercmd_s, serverTime, 0x0);
STATIC_ASSERT_OFFSET(usercmd_s, buttons, 0x4);
STATIC_ASSERT_OFFSET(usercmd_s, angles, 0xC);
STATIC_ASSERT_OFFSET(usercmd_s, weapon, 0x18);

struct pmove_t
{
	playerState_s *ps;
	usercmd_s cmd;

	// The rest omitted
};
STATIC_ASSERT_OFFSET(pmove_t, ps, 0x0);
STATIC_ASSERT_OFFSET(pmove_t, cmd, 0x4);

struct pml_t
{
	float forward[3];
	float right[3];
	float up[3];
	float frametime;
	int msec;

	// The rest omitted
};
STATIC_ASSERT_OFFSET(pml_t, msec, 0x28);

extern void(*PM_WeaponUseAmmo)(int ps, int wp, int amount);

void hk_PM_WeaponUseAmmo(int ps, int wp, int amount);
void PM_Weapon_Jam(int ps);
void PM_Weapon_FinishRechamber(playerState_s *ps);
int PM_WeaponAmmoAvailable(playerState_s *ps);

void PM_StartWeaponAnim(playerState_s *ps, int anim, int leftAnim);
void PM_ContinueWeaponAnim(playerState_s *ps, int anim, int leftAnim);
int *BG_GetWeaponState(playerState_s *ps, bool leftGun);
int *BG_GetWeaponTime(playerState_s *ps, bool leftGun);
int *BG_GetWeaponDelay(playerState_s *ps, bool leftGun);
unsigned int *BG_GetWeaponShotCount(playerState_s *ps, bool leftGun);
unsigned int PM_GetWeaponIndexForHand(playerState_s *ps);
bool BG_CanFastSwitch(WeaponDef *weapDef, int weaponState);
bool ShotLimitReached(playerState_s *ps, WeaponDef *weapDef);
bool BurstFirePending(playerState_s *ps);
bool WeaponUsesBurstCooldown(unsigned int weaponIdx);
int PM_Weapon_WeaponTimeAdjust(pmove_t *pm, pml_t *pml);