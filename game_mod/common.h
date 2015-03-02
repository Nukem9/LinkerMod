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