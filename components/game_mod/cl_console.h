#pragma once

static field_t *g_consoleField = (field_t *)0x00C72280;

VANILLA_FUNC(Con_Close, void (__cdecl*)(int localClientNum), 0x004B8020);

void Con_ToggleConsole();
bool Con_HasActiveAutoComplete();
bool Con_CancelAutoComplete();
