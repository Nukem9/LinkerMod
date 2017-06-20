#include "stdafx.h"

// /game_mp/g_client_script_cmd_mp.cpp:4125
void PlayerCmd_SetPerk(scr_entref_t entref)
{
	gentity_s *pSelf = nullptr;

	if (entref.classnum)
		Scr_ObjectError("not an entity", SCRIPTINSTANCE_SERVER);

	ASSERT(entref.entnum < MAX_GENTITIES);

	pSelf = &g_entities[entref.entnum];

	if (!pSelf->client)
		Scr_ObjectError(va("entity %i is not a player", entref.entnum), SCRIPTINSTANCE_SERVER);

	const char *perkName = Scr_GetString(0, SCRIPTINSTANCE_SERVER);
	unsigned int perkIndex = BG_GetPerkIndexForName(perkName);

	if (perkIndex == PERK_UNKNOWN)
		Scr_Error(va("Unknown perk: %s\n", perkName), false);

	BG_SetPerk(pSelf->client->ps.perks, perkIndex);
	BG_SetPerk((unsigned int *)((DWORD)pSelf->client + 0x1B64), perkIndex);
}

// /game_mp/g_client_script_cmd_mp.cpp:4155
void PlayerCmd_HasPerk(scr_entref_t entref)
{
	gentity_s *pSelf = nullptr;

	if (entref.classnum)
		Scr_ObjectError("not an entity", SCRIPTINSTANCE_SERVER);

	ASSERT(entref.entnum < MAX_GENTITIES);

	pSelf = &g_entities[entref.entnum];

	if (!pSelf->client)
		Scr_ObjectError(va("entity %i is not a player", entref.entnum), SCRIPTINSTANCE_SERVER);

	const char *perkName = Scr_GetString(0, SCRIPTINSTANCE_SERVER);
	unsigned int perkIndex = BG_GetPerkIndexForName(perkName);

	if (perkIndex == PERK_UNKNOWN)
		Scr_Error(va("Unknown perk: %s\n", perkName), false);

	Scr_AddBool(BG_HasPerk(pSelf->client->ps.perks, perkIndex), SCRIPTINSTANCE_SERVER);
}

// /game_mp/g_client_script_cmd_mp.cpp:4210
void PlayerCmd_UnsetPerk(scr_entref_t entref)
{
	gentity_s *pSelf = nullptr;

	if (entref.classnum)
		Scr_ObjectError("not an entity", SCRIPTINSTANCE_SERVER);

	ASSERT(entref.entnum < MAX_GENTITIES);

	pSelf = &g_entities[entref.entnum];

	if (!pSelf->client)
		Scr_ObjectError(va("entity %i is not a player", entref.entnum), SCRIPTINSTANCE_SERVER);

	const char *perkName = Scr_GetString(0, SCRIPTINSTANCE_SERVER);
	unsigned int perkIndex = BG_GetPerkIndexForName(perkName);

	if (perkIndex == PERK_UNKNOWN)
		Scr_Error(va("Unknown perk: %s\n", perkName), false);

	BG_UnsetPerk(pSelf->client->ps.perks, perkIndex);
	BG_UnsetPerk((unsigned int *)((DWORD)pSelf->client + 0x1B64), perkIndex);
}