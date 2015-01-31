#pragma once

struct field_t
{
	int cursor;
	int scroll;
	int drawWidth;
	int widthInPixels;
	float charHeight;
	int fixedSize;
	char buffer[256];
};

typedef void (__cdecl * Field_Clear_t)(field_t *edit);
static Field_Clear_t Field_Clear = (Field_Clear_t)0x50DB60;

static field_t *g_consoleField = (field_t *)0xC72280;

void Con_ToggleConsole();