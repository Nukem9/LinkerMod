#pragma once

void Remote_EnableUpdates(bool Enable, bool ResetChanges = false);
void Remote_CommitChanges();
void Remote_AddEntityCreate(entity_s *Entity);
void Remote_AddEntityUpdate(entity_s *Entity);
void Remote_AddEntityEpairUpdate(epair_s *Pairs);
void Remote_AddEntityDelete(entity_s *Entity);