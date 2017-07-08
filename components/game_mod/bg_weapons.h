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
	WEAPON_RECHAMBER_START = 0x7,
	WEAPON_RECHAMBERING = 0x8,
	WEAPON_RELOADING_RIGHT = 0x9,
	WEAPON_RELOADING_LEFT = 0xA,
	WEAPON_RELOADING = 0xB,
	WEAPON_RELOADING_INTERUPT = 0xC,
	WEAPON_RELOAD_START = 0xD,
	WEAPON_RELOAD_START_INTERUPT = 0xE,
	WEAPON_RELOAD_END = 0xF,
	WEAPON_RELOAD_QUICK = 0x10,
	WEAPON_RELOAD_QUICK_EMPTY = 0x11,
	WEAPON_MELEE_INIT = 0x12,
	WEAPON_MELEE_FIRE = 0x13,
	WEAPON_MELEE_END = 0x14,
	WEAPON_OFFHAND_INIT = 0x15,
	WEAPON_OFFHAND_PREPARE = 0x16,
	WEAPON_OFFHAND_HOLD = 0x17,
	WEAPON_OFFHAND_START = 0x18,
	WEAPON_OFFHAND = 0x19,
	WEAPON_OFFHAND_END = 0x1A,
	WEAPON_DETONATING = 0x1B,
	WEAPON_SPRINT_RAISE = 0x1C,
	WEAPON_SPRINT_LOOP = 0x1D,
	WEAPON_SPRINT_DROP = 0x1E,
	WEAPON_CONT_FIRE_IN = 0x1F,
	WEAPON_CONT_FIRE_LOOP = 0x20,
	WEAPON_CONT_FIRE_OUT = 0x21,
	WEAPON_NIGHTVISION_WEAR = 0x22,
	WEAPON_NIGHTVISION_REMOVE = 0x23,
	WEAPON_DEPLOYING = 0x24,
	WEAPON_DEPLOYED = 0x25,
	WEAPON_BREAKING_DOWN = 0x26,
	WEAPON_SWIM_IN = 0x27,
	WEAPON_SWIM_OUT = 0x28,
	WEAPON_DTP_IN = 0x29,
	WEAPON_DTP_LOOP = 0x2A,
	WEAPON_DTP_OUT = 0x2B,
	WEAPON_SLIDE_IN = 0x2C,
	WEAPON_FIRING_LEFT = 0x2D,
	WEAPON_FIRING_BOTH = 0x2E,
	WEAPON_JAMMED = 0x2F,
	WEAPON_LOWREADY_RAISE = 0x30,
	WEAPON_LOWREADY_LOOP = 0x31,
	WEAPON_LOWREADY_DROP = 0x32,
	WEAPONSTATES_NUM = 0x33,
};

#define IS_WEAPONSTATE_RELOAD(x) (\
								(x) == WEAPON_RELOADING || \
								(x) == WEAPON_RELOADING_INTERUPT || \
								(x) == WEAPON_RELOAD_START || \
								(x) == WEAPON_RELOAD_START_INTERUPT || \
								(x) == WEAPON_RELOAD_END || \
								(x) == WEAPON_RELOAD_QUICK || \
								(x) == WEAPON_RELOAD_QUICK_EMPTY)

#define IS_WEAPONSTATE_MELEE(x) (\
								(x) == WEAPON_MELEE_INIT || \
								(x) == WEAPON_MELEE_FIRE || \
								(x) == WEAPON_MELEE_END)

#define IS_WEAPONSTATE_FIRE(x) (\
								(x) == WEAPON_FIRING || \
								(x) == WEAPON_RECHAMBER_START || \
								(x) == WEAPON_RECHAMBERING || \
								(x) == WEAPON_CONT_FIRE_LOOP || \
								IS_WEAPONSTATE_MELEE(x))

struct usercmd_s
{
	int serverTime;
	bitarray<51> buttons;
	int angles[3];
	unsigned char weapon;

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

void PM_StartWeaponAnim(playerState_s *ps, int anim, int leftAnim);
void PM_ContinueWeaponAnim(playerState_s *ps, int anim, int leftAnim);
int *BG_GetWeaponState(playerState_s *ps, bool leftGun);
int *BG_GetWeaponTime(playerState_s *ps, bool leftGun);
int *BG_GetWeaponDelay(playerState_s *ps, bool leftGun);
unsigned int *BG_GetWeaponShotCount(playerState_s *ps, bool leftGun);
void PM_Weapon_FinishRechamber(playerState_s *ps);
unsigned int PM_GetWeaponIndexForHand(playerState_s *ps);
bool BG_CanFastSwitch(WeaponDef *weapDef, int weaponState);
bool ShotLimitReached(playerState_s *ps, WeaponDef *weapDef);
bool BurstFirePending(playerState_s *ps);
bool WeaponUsesBurstCooldown(unsigned int weaponIdx);
int PM_Weapon_WeaponTimeAdjust(pmove_t *pm, pml_t *pml);
void PM_Weapon_Jam(/*playerState_s *ps*/);
WeaponVariantDef *BG_LoadWeaponVariantDef(const char *name);
const char *BG_WeaponName(int weapon);

// /bgame/bg_weapons.h:300
static int BG_GetHeldWeaponSlot(playerState_s *ps, int weapon)
{
	ASSERT(ps);

	for (int i = 0; i < ARRAYSIZE(ps->heldWeapons); i++)
	{
		if (ps->heldWeapons[i].weapon == weapon)
			return i;
	}

	return -1;
}

// /bgame/bg_weapons.h:???
static PlayerHeldWeapon *BG_GetHeldWeapon(playerState_s *ps, int weapon)
{
	ASSERT(ps);

	if (weapon == 0)
		return nullptr;

	int index = BG_GetHeldWeaponSlot(ps, weapon);

	if (index == -1)
		return nullptr;

	return &ps->heldWeapons[index];
}