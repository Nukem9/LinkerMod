#pragma once

static field_t *g_consoleField = (field_t *)0x00C72280;

void Con_ToggleConsole();
bool Con_HasActiveAutoComplete();
bool Con_CancelAutoComplete();