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

extern void (__cdecl * Com_Init)(char *commandLine);

void hk_Com_Init(char *commandLine);
void Field_Clear(field_t *edit);

typedef void (__cdecl* Com_PrintWarning_t)(int channel, const char *fmt, ...);
static Com_PrintWarning_t Com_PrintWarning = (Com_PrintWarning_t)0x0051CE50;