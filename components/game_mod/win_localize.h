#pragma once

#define GM_USE_VERBOSE_EXCEPTION 1

typedef const char *(__cdecl* Win_LocalizeRef_t)(const char *ref);
static Win_LocalizeRef_t Win_LocalizeRef = (Win_LocalizeRef_t)0x00527B50;
