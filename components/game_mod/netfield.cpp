#include "stdafx.h"

NetFieldList s_entityNetFieldList[21] =
{
	{ (NetField *)0x00A56808, 0x44 },	// ET_GENERAL			eventEntityStateFields
	{ (NetField *)0x00A57D48, 0x48 },	// ET_PLAYER			playerEntityStateFields
	{ (NetField *)0x00A582E8, 0x43 },	// ET_PLAYER_CORPSE		corpseEntityStateFields
	{ (NetField *)0x00A59DF8, 0x44 },	// ET_ITEM				itemEntityStateFields
	{ (NetField *)0x00A5B8A0, 0x44 },	// ET_MISSLE			missileEntityStateFields
	{ (NetField *)0x00A56808, 0x44 },	// ET_INVISIBLE			entityStateFields
	{ (NetField *)0x00A5A348, 0x49 },	// ET_SCRIPTMOVER		scriptMoverStateFields
	{ (NetField *)0x00A5A900, 0x42 },	// ET_SOUND_BLEND		soundBlendEntityStateFields
	{ (NetField *)0x00A5AE28, 0x42 },	// ET_FX				fxStateFields
	{ (NetField *)0x00A5B350, 0x44 },	// ET_LOOP_FX			loopFxEntityStateFields
	{ (NetField *)0x00A572A8, 0x44 },	// ET_PRIMARY_LIGHT		entityStateFields
	{ (NetField *)0x00A56D58, 0x44 },	// ET_MG42				turretEntityStateFields
	// ET_HELICOPTER (removed in SP?)
	{ (NetField *)0x00A598B8, 0x43 },	// ET_PLANE				planeStateFields
	{ (NetField *)0x00A592D8, 0x4B },	// ET_VEHICLE			vehicleEntityStateFields
	{ (NetField *)0x00A56808, 0x44 },	// ET_VEHICLE_CORPSE	entityStateFields
	{ (NetField *)0x00A56808, 0x44 },	// ET_VEHICLE_COLLMAP	entityStateFields
	{ (NetField *)0x00A58838, 0x44 },	// ET_ACTOR				actorStateFields
	{ (NetField *)0x00A56808, 0x44 },	// ET_ACTOR_SPAWNER		actorStateFields
	{ (NetField *)0x00A58D88, 0x44 },	// ET_ACTOR_CORPSE		actorStateFields
	{ (NetField *)0x00A56808, 0x44 },	// ET_STREAMER_HINT		entityStateFields
	{ (NetField *)0x00A577F8, 0x44 },	// ET_EVENTS			eventEntityStateFields
};

NetFieldList s_otherNetFieldList[8] =
{
	{ (NetField *)0x00A5BDF0, 0x08 },	// NET_FIELD_TYPE_ARCHIVED_ENTITY
	{ (NetField *)0x00A5BE90, 0x2C },	// NET_FIELD_TYPE_CLIENT_STATE
	{ (NetField *)0x00A5C200, 0x1E },	// NET_FIELD_TYPE_ACTORSTATE
	{ (NetField *)0x00A5C458, 0x0A },	// ?
	{ (NetField *)0x00A5C520, 0xB2 },	// NET_FIELD_TYPE_PLAYERSTATE
	{ (NetField *)0x00A5D308, 0x06 },	// NET_FIELD_TYPE_OBJECTIVE
	{ (NetField *)0x00A5D380, 0x2D },	// NET_FIELD_TYPE_HUDELEMS
	{ (NetField *)0x00A58824, 0x01 },	// ?
};

void Cmd_DumpNetfield_f()
{
	if (Cmd_Argc() != 3)
	{
		Com_Printf(0, "Usage: dump_netfield <entity|other> <index|all>\n");
		return;
	}

	const char *type = Cmd_Argv(1);
	const char *index = Cmd_Argv(2);

	// Validate field type the user wanted
	NetFieldList *list = nullptr;
	int listMaxIndex = 0;

	if (!_stricmp(type, "entity"))
	{
		list = s_entityNetFieldList;
		listMaxIndex = ARRAYSIZE(s_entityNetFieldList);
	}
	else if (!_stricmp(type, "other"))
	{
		list = s_otherNetFieldList;
		listMaxIndex = ARRAYSIZE(s_otherNetFieldList);
	}
	else
	{
		Com_Printf(0, "Unknown netfield type\n");
		return;
	}

	// Validate index
	int listUseIndex = -1;

	if (_stricmp(index, "all") != 0)
	{
		listUseIndex = atoi(index);

		if (listUseIndex < 0 || listUseIndex >= listMaxIndex)
		{
			Com_Printf(0, "Invalid netfield array index\n");
			return;
		}
	}

	// Now print everything to the console
	for (int i = 0; i < listMaxIndex; i++)
	{
		if (listUseIndex != -1 && i != listUseIndex)
			continue;

		Com_Printf(1, "\nNetFieldList: %i\n", i);

		for (unsigned int j = 0; j < list[i].count; j++)
		{
			Com_Printf(1, "0x%04X %d 0x%08X %d: %s\n",
				list[i].array[j].offset,
				list[i].array[j].size,
				list[i].array[j].bits,
				(int)list[i].array[j].changeHints,
				list[i].array[j].name);
		}
	}
}