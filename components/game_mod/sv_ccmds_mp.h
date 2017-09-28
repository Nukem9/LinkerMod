#pragma once

void SV_KillServer_f();
void SV_MapRestart_f();

extern void* SV_ValidateMap_o;
void SV_ValidateMap();

typedef bool(__cdecl* PartyClient_CheckMapExists_t)(const char *map);
extern PartyClient_CheckMapExists_t PartyClient_CheckMapExists_o;

bool __cdecl PartyClient_CheckMapExists(const char *map);

void SV_SetPerk_f();
