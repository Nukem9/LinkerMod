#include "stdafx.h"

std::unordered_map<entity_s *, bool> g_PendingEntityUpdates;
std::unordered_map<entity_s *, std::string> g_UpdateStrings;

bool g_EnableUpdates = true;
bool g_CreatingEntity = false;

void CreateEntityData2(entity_s *ent, char *buf, int len)
{
	char temp[512];
	memset(temp, 0, sizeof(temp));
	memset(buf, 0, len * sizeof(char));

	strcat_s(buf, len, "{\n");

	// Append each key and value
	for (epair_s *pair = ent->epairs; pair; pair = pair->next)
	{
		// Skip origin since it is not synced all the time
		if (!_stricmp(pair->key, "origin"))
			continue;

		sprintf_s(temp, "\"%s\" \"%s\"\n", pair->key, pair->value);
		strcat_s(buf, len, temp);
	}

	// Append origin manually
	if (!strstr(buf, "\"origin\""))
	{
		sprintf_s(temp, "\"origin\" \"%.4f %.4f %.4f\"\n", ent->origin[0], ent->origin[1], ent->origin[2]);
		strcat_s(buf, len, temp);
	}

	strcat_s(buf, len, "}\n");
}

void Remote_EnableUpdates(bool Enable, bool ResetChanges)
{
	g_EnableUpdates = Enable;

	if (ResetChanges)
	{
		g_UpdateStrings.clear();
		g_PendingEntityUpdates.clear();
	}
}

void Remote_CommitChanges()
{
	if (!g_EnableUpdates)
		return;

	for (auto& itr : g_PendingEntityUpdates)
	{
		char buf[1024];
		CreateEntityData2(itr.first, buf, 1024);

		RemoteNet_SendSelect(g_UpdateStrings[itr.first].c_str());
		RemoteNet_SendUpdate(buf);
	}

	// Restore the original selection
	// TODO RemoteNet_SendSelect

	Remote_EnableUpdates(true, true);
	g_CreatingEntity = false;
}

void Remote_AddEntityCreate(entity_s *Entity)
{
	if (!g_EnableUpdates)
		return;
}

void Remote_AddEntityUpdate(entity_s *Entity)
{
	if (!g_EnableUpdates)
		return;

	// We don't want the internal project entity (not actually a world entity)
	if (Entity == g_qeglobals_d_project_entity)
		return;

	char buf[512];
	CreateEntityData2(Entity, buf, ARRAYSIZE(buf));

	g_UpdateStrings[Entity] = buf;
	g_PendingEntityUpdates[Entity] = true;
}

void Remote_AddEntityEpairUpdate(epair_s *Pairs)
{
	if (!g_EnableUpdates)
		return;

	//
	// We need to scan the entire entity list to find out
	// who this epair pointer belongs to.
	//
	// Use a cached search to (hopefully) skip iterating the
	// entire world.
	//
	bool found = false;

	for (auto& itr : g_PendingEntityUpdates)
	{
		if (itr.first->epairs == Pairs)
		{
			found = true;
			break;
		}
	}

	if (found)
		return;

	// NOTE: This search does include worldspawn
	entity_s *itr = world_entity;

	if (itr)
	{
		do
		{
			// Continue scanning even if we found a match
			if (itr->epairs == Pairs)
				Remote_AddEntityUpdate(itr);

			itr = itr->next;
		} while ((itr != nullptr) && (itr != world_entity));
	}
}

void Remote_AddEntityDelete(entity_s *Entity)
{
	if (!g_EnableUpdates)
		return;
}