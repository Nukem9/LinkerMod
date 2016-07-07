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

typedef void(__cdecl* Com_Printf_t)(int channel, const char *fmt, ...);
static Com_Printf_t Com_Printf = (Com_Printf_t)0x0043BF30;
static Com_Printf_t Com_PrintWarning = (Com_Printf_t)0x0051CE50;

//
// Used to print to Launcher's console, as well as the game's console / logfile
//
void Com_ToolPrintf(int channel, const char* fmt, ...);

typedef char *va_t(const char *format, ...);
static va_t* va = (va_t*)0x0057CDD0;

int nullsub(int);