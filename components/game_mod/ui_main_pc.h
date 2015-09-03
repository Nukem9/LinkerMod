#pragma once

bool __cdecl UI_LoadModArenas();

typedef int (__cdecl* UI_ParseInfos_t)(const char *buf, int max, char **infos);
static UI_ParseInfos_t UI_ParseInfos = (UI_ParseInfos_t)0x005EF550;

typedef const char *__cdecl UI_SafeTranslateString_t(const char *reference);
static UI_SafeTranslateString_t* UI_SafeTranslateString = (UI_SafeTranslateString_t*)0x006664D0;

typedef void __cdecl Live_UpdateUiPopup_t(int unk, const char *popupname);
static Live_UpdateUiPopup_t* Live_UpdateUiPopup = (Live_UpdateUiPopup_t*)0x005C8FE0;