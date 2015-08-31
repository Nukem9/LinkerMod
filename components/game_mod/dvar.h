#pragma once

enum dvarType_t
{
	DVAR_TYPE_BOOL = 0x0,
	DVAR_TYPE_FLOAT = 0x1,
	DVAR_TYPE_FLOAT_2 = 0x2,
	DVAR_TYPE_FLOAT_3 = 0x3,
	DVAR_TYPE_FLOAT_4 = 0x4,
	DVAR_TYPE_INT = 0x5,
	DVAR_TYPE_ENUM = 0x6,
	DVAR_TYPE_STRING = 0x7,
	DVAR_TYPE_COLOR = 0x8,
	DVAR_TYPE_INT64 = 0x9,
	DVAR_TYPE_LINEAR_COLOR_RGB = 0xA,
	DVAR_TYPE_COLOR_XYZ = 0xB,
	DVAR_TYPE_COUNT = 0xC,
};

union DvarValue
{
	bool enabled;
	int integer;
	unsigned int unsignedInt;
	__int64 integer64;
	unsigned __int64 unsignedInt64;
	float value;
	float vector[4];
	const char *string;
	char color[4];
};

union DvarLimits
{
	struct
	{
		int stringCount;
		const char **strings;
	} enumeration;

	struct
	{
		int min;
		int max;
	} integer;

	struct
	{
		__int64 min;
		__int64 max;
	} integer64;

	struct
	{
		float min;
		float max;
	} value;

	struct
	{
		float min;
		float max;
	} vector;
};

struct dvar_s
{
	const char *name;
	const char *description;
	int hash;
	unsigned int flags;
	dvarType_t type;
	bool modified;
	bool loadedFromSaveGame;
	DvarValue current;
	DvarValue latched;
	DvarValue reset;
	DvarValue saved;
	DvarLimits domain;
	dvar_s *hashNext;
};

static dvar_s **com_sv_running = (dvar_s **)0x0243FD3C;

typedef const char* Dvar_GetString_t(const char* dvarName);
static Dvar_GetString_t* Dvar_GetString = (Dvar_GetString_t*)0x0057FF80;

typedef void __cdecl Dvar_SetString_t(dvar_s *dvar, const char *value);
static Dvar_SetString_t* Dvar_SetString = (Dvar_SetString_t*)0x0044A2A0;

typedef void __cdecl Dvar_SetStringByName_t(const char *dvarName, const char *value);
static Dvar_SetStringByName_t* Dvar_SetStringByName = (Dvar_SetStringByName_t*)0x00666F30;
