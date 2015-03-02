#pragma once

static field_t *g_consoleField = (field_t *)0xC72280;

bool Con_HasActiveAutoComplete();
bool Con_CancelAutoComplete();
void Con_ToggleConsole();