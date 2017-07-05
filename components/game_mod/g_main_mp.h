#pragma once

#define MAX_GENTITIES 1024

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
	char _pad2[0x200];
};
STATIC_ASSERT_OFFSET(gentity_s, client, 0x13C);
STATIC_ASSERT_OFFSET(gentity_s, vehicle, 0x148);
STATIC_ASSERT_SIZE(gentity_s, 0x34C);

struct renderOptions_s
{
	union
	{
		unsigned int i;

		struct
		{
			int _bf0;
		};
	};
};

struct PlayerHeldWeapon
{
	unsigned int weapon;
	unsigned char model;
	renderOptions_s options;

	union
	{
		float heatPercent;
		int fuelTankTime;
	};

	bool overHeating;
	bool needsRechamber;
	bool heldBefore;
	bool quickReload;
	bool blockWeaponPickup;
};
STATIC_ASSERT_OFFSET(PlayerHeldWeapon, weapon, 0x0);
STATIC_ASSERT_OFFSET(PlayerHeldWeapon, needsRechamber, 0x11);
STATIC_ASSERT_SIZE(PlayerHeldWeapon, 0x18);

struct AmmoPool
{
	int	ammoIndex;
	int	count;
};
STATIC_ASSERT_SIZE(AmmoPool, 0x8);

struct AmmoClip
{
	int	clipIndex;
	int	count;
};
STATIC_ASSERT_SIZE(AmmoClip, 0x8);

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
	char _pad9[0x28];
	PlayerHeldWeapon heldWeapons[15];
	AmmoPool ammoNotInClip[15];
	AmmoClip ammoInClip[15];
	char _pad10[0xC8];
	unsigned int perks[1];
	char _pad11[0x24];
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
STATIC_ASSERT_OFFSET(playerState_s, heldWeapons, 0x1DC);
STATIC_ASSERT_OFFSET(playerState_s, ammoNotInClip, 0x344);
STATIC_ASSERT_OFFSET(playerState_s, ammoInClip, 0x3BC);
STATIC_ASSERT_OFFSET(playerState_s, perks, 0x4FC);
STATIC_ASSERT_OFFSET(playerState_s, weapAnim, 0x524);
STATIC_ASSERT_OFFSET(playerState_s, weapAnimLeft, 0x528);
//STATIC_ASSERT_SIZE(playerState_s, 0x0);

struct clientState_s
{
	char _pad1[0x7C];
	unsigned int perks[1];
	char _pad2[0x1CA8];
};
STATIC_ASSERT_OFFSET(clientState_s, perks, 0x7C);
STATIC_ASSERT_SIZE(clientState_s, 0x1D28);

struct clientSession_t
{
};
//STATIC_ASSERT_SIZE(clientSession_t, 0x0);

struct gclient_s
{
	playerState_s ps;
	//clientSession_t sess;
};
STATIC_ASSERT_OFFSET(gclient_s, ps, 0x0);
//STATIC_ASSERT_SIZE(gclient_s, 0x0);

struct clientHeader_t
{
	int state;
	char _pad1[0x20];
	struct
	{
		struct
		{
			int type;
		} remoteAddress;
	} netchan;
	char _pad2[0x6F0];
};
STATIC_ASSERT_OFFSET(clientHeader_t, state, 0x0);
//STATIC_ASSERT_OFFSET(clientHeader_t, netchan, 0x24);
STATIC_ASSERT_SIZE(clientHeader_t, 0x718);

struct client_t
{
	clientHeader_t header;
	const char *dropReason;
	BYTE gap28[0x10850];
	int lastClientCommand;
	char lastClientCommandString[1024];
	gentity_s *gentity;
	char name[32];
	BYTE gap11394[184];
	int nextReliableTime;
	int nextReliableCount;
	char _pad1[0x2F26C];
};
STATIC_ASSERT_OFFSET(client_t, header, 0x0);
STATIC_ASSERT_OFFSET(client_t, dropReason, 0x718);
STATIC_ASSERT_OFFSET(client_t, lastClientCommand, 0x10F6C);
STATIC_ASSERT_OFFSET(client_t, lastClientCommandString, 0x10F70);
STATIC_ASSERT_OFFSET(client_t, gentity, 0x11370);
STATIC_ASSERT_OFFSET(client_t, name, 0x11374);
STATIC_ASSERT_OFFSET(client_t, nextReliableTime, 0x1144C);
STATIC_ASSERT_OFFSET(client_t, nextReliableCount, 0x11450);
STATIC_ASSERT_SIZE(client_t, 0x406C0);

static gentity_s *g_entities = (gentity_s *)0x01A796F8;