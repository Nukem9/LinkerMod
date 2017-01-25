#pragma once

struct gentity_s;
struct gclient_s;
struct scr_vehicle_s;

struct entityState_s
{
	int number;
	char _pad1[0xBA];
	__int16 eType;
	char _pad2[0x18];
};
STATIC_ASSERT_OFFSET(entityState_s, number, 0x0);
STATIC_ASSERT_OFFSET(entityState_s, eType, 0xBE);
STATIC_ASSERT_SIZE(entityState_s, 0xD8);

struct entityShared_t
{
	char _pad1[0x64];
};
STATIC_ASSERT_SIZE(entityShared_t, 0x64);

struct gentity_s
{
	entityState_s s;
	entityShared_t r;
	gclient_s *client;
	char _pad1[0x8];
	scr_vehicle_s *vehicle;
};
STATIC_ASSERT_OFFSET(gentity_s, client, 0x13C);
STATIC_ASSERT_OFFSET(gentity_s, vehicle, 0x148);

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
	char _pad5[0xCE];
	char clientNum;
	char _pad6[0x13];
	unsigned char weapon;
	char _pad7[0x12];
	int weaponstate;
	int weaponstateLeft;
	unsigned int weaponShotCount;
	unsigned int weaponShotCountLeft;
	char _pad8[0x48];
	unsigned int stackFireCount;
	char _pad9[0x348];
	unsigned int perks[1];
	char _pad10[0x24];
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
STATIC_ASSERT_OFFSET(playerState_s, clientNum, 0x130);
STATIC_ASSERT_OFFSET(playerState_s, weapon, 0x144);
STATIC_ASSERT_OFFSET(playerState_s, weaponstate, 0x158);
STATIC_ASSERT_OFFSET(playerState_s, weaponstateLeft, 0x15C);
STATIC_ASSERT_OFFSET(playerState_s, weaponShotCount, 0x160);
STATIC_ASSERT_OFFSET(playerState_s, weaponShotCountLeft, 0x164);
STATIC_ASSERT_OFFSET(playerState_s, stackFireCount, 0x1B0);
STATIC_ASSERT_OFFSET(playerState_s, perks, 0x4FC);
STATIC_ASSERT_OFFSET(playerState_s, weapAnim, 0x524);
STATIC_ASSERT_OFFSET(playerState_s, weapAnimLeft, 0x528);

struct gclient_s
{
	playerState_s ps;
};
STATIC_ASSERT_OFFSET(gclient_s, ps, 0x0);