#pragma once

typedef bool(__cdecl *MapEnts_CustomEntsFile_IsPresent_t)();
static MapEnts_CustomEntsFile_IsPresent_t MapEnts_CustomEntsFile_IsPresent = (MapEnts_CustomEntsFile_IsPresent_t)0x0041F6E0;

void mfh_MapEnts_ParseEntities();
