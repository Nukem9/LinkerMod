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
	char _pad2[0x38];
	int health;
	int maxHealth;
	char _pad3[0x1C0];
};
STATIC_ASSERT_OFFSET(gentity_s, client, 0x13C);
STATIC_ASSERT_OFFSET(gentity_s, vehicle, 0x148);
STATIC_ASSERT_OFFSET(gentity_s, health, 0x184);
STATIC_ASSERT_OFFSET(gentity_s, maxHealth, 0x188);
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
	char _pad1[0x4];
	int pm_flags;
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
	char _pad5[0x2E];
	int groundEntityNum;
	char _pad6[0x9C];
	char clientNum;
	char _pad7[0x13];
	unsigned char weapon;
	char _pad8[0x12];
	int weaponstate;
	int weaponstateLeft;
	unsigned int weaponShotCount;
	unsigned int weaponShotCountLeft;
	char _pad9[0x10];
	float weaponSpinLerp;
	char _pad10[0x34];
	unsigned int stackFireCount;
	char _pad11[0x28];
	PlayerHeldWeapon heldWeapons[15];
	AmmoPool ammoNotInClip[15];
	AmmoClip ammoInClip[15];
	char _pad12[0xC8];
	unsigned int perks[1];
	char _pad13[0x24];
	int weapAnim;
	int weapAnimLeft;
	char _pad14[0x14E8];
};
STATIC_ASSERT_OFFSET(playerState_s, commandTime, 0x0);
STATIC_ASSERT_OFFSET(playerState_s, pm_type, 0x4);
STATIC_ASSERT_OFFSET(playerState_s, pm_flags, 0xC);
STATIC_ASSERT_OFFSET(playerState_s, weapFlags, 0x10);
STATIC_ASSERT_OFFSET(playerState_s, weaponTime, 0x3C);
STATIC_ASSERT_OFFSET(playerState_s, weaponDelay, 0x40);
STATIC_ASSERT_OFFSET(playerState_s, weaponTimeLeft, 0x44);
STATIC_ASSERT_OFFSET(playerState_s, weaponDelayLeft, 0x48);
STATIC_ASSERT_OFFSET(playerState_s, weaponRestrictKickTime, 0x5C);
STATIC_ASSERT_OFFSET(playerState_s, bRunLeftGun, 0x61);
STATIC_ASSERT_OFFSET(playerState_s, groundEntityNum, 0x90);
STATIC_ASSERT_OFFSET(playerState_s, clientNum, 0x130);
STATIC_ASSERT_OFFSET(playerState_s, weapon, 0x144);
STATIC_ASSERT_OFFSET(playerState_s, weaponstate, 0x158);
STATIC_ASSERT_OFFSET(playerState_s, weaponstateLeft, 0x15C);
STATIC_ASSERT_OFFSET(playerState_s, weaponShotCount, 0x160);
STATIC_ASSERT_OFFSET(playerState_s, weaponShotCountLeft, 0x164);
STATIC_ASSERT_OFFSET(playerState_s, weaponSpinLerp, 0x178);
STATIC_ASSERT_OFFSET(playerState_s, stackFireCount, 0x1B0);
STATIC_ASSERT_OFFSET(playerState_s, heldWeapons, 0x1DC);
STATIC_ASSERT_OFFSET(playerState_s, ammoNotInClip, 0x344);
STATIC_ASSERT_OFFSET(playerState_s, ammoInClip, 0x3BC);
STATIC_ASSERT_OFFSET(playerState_s, perks, 0x4FC);
STATIC_ASSERT_OFFSET(playerState_s, weapAnim, 0x524);
STATIC_ASSERT_OFFSET(playerState_s, weapAnimLeft, 0x528);
STATIC_ASSERT_SIZE(playerState_s, 0x1A14);

struct score_s
{
	int ping;
	int status_icon;
	int place;
	int score;
	int kills;
	int assists;
	char _pad1[0x10];
	int downs;
	int revives;
	int headshots;
	int scoreMultiplier;
};
STATIC_ASSERT_OFFSET(score_s, ping, 0x0);
STATIC_ASSERT_OFFSET(score_s, status_icon, 0x4);
STATIC_ASSERT_OFFSET(score_s, place, 0x8);
STATIC_ASSERT_OFFSET(score_s, score, 0xC);
STATIC_ASSERT_OFFSET(score_s, kills, 0x10);
STATIC_ASSERT_OFFSET(score_s, assists, 0x14);
STATIC_ASSERT_OFFSET(score_s, downs, 0x28);
STATIC_ASSERT_OFFSET(score_s, revives, 0x2C);
STATIC_ASSERT_OFFSET(score_s, headshots, 0x30);
STATIC_ASSERT_OFFSET(score_s, scoreMultiplier, 0x34);

struct clientState_s
{
	int clientIndex;
	int team;
	int modelindex;
	int attachModelIndex[6];
	int attachTagIndex[6];
	int lastDamageTime;
	int lastStandStartTime;
	int beingRevived;
	char _pad1[0x8];
	char name[32];
	float maxSprintTimeMultiplier;
	int rank;
	int prestige;
	unsigned int perks[1];
	char clanAbbrev[8];
	int attachedEntNum;
	int attachedTagIndex;
	int vehAnimState;
	score_s score;
	char _pad2[0x4];
};
STATIC_ASSERT_OFFSET(clientState_s, clientIndex, 0x0);
STATIC_ASSERT_OFFSET(clientState_s, team, 0x4);
STATIC_ASSERT_OFFSET(clientState_s, modelindex, 0x8);
STATIC_ASSERT_OFFSET(clientState_s, attachModelIndex, 0xC);
STATIC_ASSERT_OFFSET(clientState_s, attachTagIndex, 0x24);
STATIC_ASSERT_OFFSET(clientState_s, lastDamageTime, 0x3C);
STATIC_ASSERT_OFFSET(clientState_s, lastStandStartTime, 0x40);
STATIC_ASSERT_OFFSET(clientState_s, beingRevived, 0x44);
STATIC_ASSERT_OFFSET(clientState_s, name, 0x50);
STATIC_ASSERT_OFFSET(clientState_s, maxSprintTimeMultiplier, 0x70);
STATIC_ASSERT_OFFSET(clientState_s, rank, 0x74);
STATIC_ASSERT_OFFSET(clientState_s, prestige, 0x78);
STATIC_ASSERT_OFFSET(clientState_s, perks, 0x7C);
STATIC_ASSERT_OFFSET(clientState_s, clanAbbrev, 0x80);
STATIC_ASSERT_OFFSET(clientState_s, attachedEntNum, 0x88);
STATIC_ASSERT_OFFSET(clientState_s, attachedTagIndex, 0x8C);
STATIC_ASSERT_OFFSET(clientState_s, vehAnimState, 0x90);
STATIC_ASSERT_OFFSET(clientState_s, score, 0x94);
STATIC_ASSERT_SIZE(clientState_s, 0xD0);

struct clientSession_t
{
	char _pad1[0xD4];
	clientState_s cs;
};
STATIC_ASSERT_OFFSET(clientSession_t, cs, 0xD4);
//STATIC_ASSERT_SIZE(clientSession_t, 0x0);

struct gclient_s
{
	playerState_s ps;
	clientSession_t sess;
};
STATIC_ASSERT_OFFSET(gclient_s, ps, 0x0);
STATIC_ASSERT_OFFSET(gclient_s, sess, 0x1A14);
//STATIC_ASSERT_SIZE(gclient_s, 0x1D28);

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

struct level_locals_t
{
	gclient_s *clients;
	gentity_s *gentities;
};
STATIC_ASSERT_OFFSET(level_locals_t, clients, 0x0);
STATIC_ASSERT_OFFSET(level_locals_t, gentities, 0x4);

static gentity_s *g_entities = (gentity_s *)0x01A796F8;
static gclient_s *g_clients = (gclient_s *)0x01C08B40;
static level_locals_t& level = *(level_locals_t *)0x01C03140;

bool G_OnlyConnectingPaths();
clientState_s *G_GetClientState(int clientNum);
playerState_s *G_GetPlayerState(int clientNum);