#pragma once

typedef void* ParseThreadInfo;

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
static Com_Printf_t Com_PrintError = (Com_Printf_t)0x005DFC40;
static Com_Printf_t Com_Error = (Com_Printf_t)0x00651D90;

//
// Used to print to Launcher's console, as well as the game's console / logfile
//
void Com_ToolPrintf(int channel, const char* fmt, ...);
void Com_ToolError(int channel, const char* fmt, ...);

typedef char *va_t(const char *format, ...);
static va_t* va = (va_t*)0x0057CDD0;

int nullsub(int);


typedef const char* (__cdecl* Com_LoadInfoString_t)(const char *fileName, const char *fileDesc, const char *ident, char *loadBuffer);
static Com_LoadInfoString_t Com_LoadInfoString = (Com_LoadInfoString_t)0x005A5F10;

typedef void (__cdecl* Com_SetCSV_t)(int csv);
static Com_SetCSV_t Com_SetCSV = (Com_SetCSV_t)0x00403200;


typedef const char* (__cdecl* Com_Parse_t)(const char **data_p);
static Com_Parse_t Com_Parse = (Com_Parse_t)0x00566B30;

typedef const char *(__cdecl* Com_ParseRestOfLine_t)(const char **data_p);
static Com_ParseRestOfLine_t Com_ParseRestOfLine = (Com_ParseRestOfLine_t)0x005D4F10;

typedef void (__cdecl* Com_BeginParseSession_t)(const char *filename);
static Com_BeginParseSession_t Com_BeginParseSession = (Com_BeginParseSession_t)0x00509BE0;

typedef ParseThreadInfo *(__cdecl* Com_EndParseSession_t)();
static Com_EndParseSession_t Com_EndParseSession = (Com_EndParseSession_t)0x005C11C0;


typedef void (__cdecl* Com_LoadCommonFastFile_t)();
static Com_LoadCommonFastFile_t Com_LoadCommonFastFile = (Com_LoadCommonFastFile_t)0x0082CB50;

typedef int (__cdecl* I_strcmp_t)(const char *s0, const char *s1);
static I_strcmp_t I_strcmp = (I_strcmp_t)0x0063E630;

typedef int (__cdecl* I_strncmp_t)(const char *s0, const char *s1, int n);
static I_strncmp_t I_strncmp = (I_strncmp_t)0x005ED6F0;

typedef int (__cdecl* I_strnicmp_t)(const char *s0, const char *s1, int n);
static I_strnicmp_t I_strnicmp = (I_strnicmp_t)0x0052F790;

typedef const char *(__cdecl* I_stristr_t)(const char *s0, const char *substr);
static I_stristr_t I_stristr = (I_stristr_t)0x0062F110;

typedef void (__cdecl* I_strncpyz_t)(char *dest, const char *src, int destsize);
static I_strncpyz_t I_strncpyz = (I_strncpyz_t)0x005D4D60;

char *__cdecl Com_GetLevelSharedFastFile(const char *mapName);
void __cdecl Com_LoadLevelFastFiles(const char *mapName);
