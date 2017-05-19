#pragma once

struct scr_entref_t
{
	unsigned __int16 entnum;
	unsigned __int16 classnum;
	unsigned __int16 client;
};

enum SCR_VALUE_TYPE
{
	SCR_VALUE_UNDEFINED = 0x0,
	SCR_VALUE_OBJECT = 0x1,
	SCR_VALUE_STRING = 0x2,
	SCR_VALUE_LOCALIZEDSTRING = 0x3,
	SCR_VALUE_VEC = 0x4,
	SCR_VALUE_FLOAT = 0x5,
	SCR_VALUE_INT = 0x6,
};

////////////////////////////////////////////////

//
// The following functions get the argument properties for a builtin script function
//

// Get the passed parameter count for a builtin GSC function
typedef unsigned int (__cdecl* Scr_GetNumParam_t)(scriptInstance_t inst);
static Scr_GetNumParam_t Scr_GetNumParam = (Scr_GetNumParam_t)0x005C6DA0;

typedef int(__cdecl* Scr_GetInt_t)(unsigned int index, scriptInstance_t inst);
static Scr_GetInt_t Scr_GetInt = (Scr_GetInt_t)0x004C1BB0;

typedef float(__cdecl* Scr_GetFloat_t)(unsigned int index, scriptInstance_t inst);
static Scr_GetFloat_t Scr_GetFloat = (Scr_GetFloat_t)0x00473380;

typedef const char *(__cdecl* Scr_GetString_t)(unsigned int index, scriptInstance_t inst);
static Scr_GetString_t Scr_GetString = (Scr_GetString_t)0x00567CB0;

typedef void(__cdecl* Scr_GetVector_t)(unsigned int index, float *vectorValue, scriptInstance_t inst);
static Scr_GetVector_t Scr_GetVector = (Scr_GetVector_t)0x00462860;

//
// The following functions essentially set a return value for a builtin script function
//
typedef void(__cdecl* Scr_AddInt_t)(int value, scriptInstance_t inst);
static Scr_AddInt_t Scr_AddInt = (Scr_AddInt_t)0x0045DBB0;

typedef void(__cdecl* Scr_AddFloat_t)(float value, scriptInstance_t inst);
static Scr_AddFloat_t Scr_AddFloat = (Scr_AddFloat_t)0x0065E540;

typedef void(__cdecl* Scr_AddString_t)(const char *value, scriptInstance_t inst);
static Scr_AddString_t Scr_AddString = (Scr_AddString_t)0x005FD1F0;

typedef void(__cdecl* Scr_AddVector_t)(const float *value, scriptInstance_t inst);
static Scr_AddVector_t Scr_AddVector = (Scr_AddVector_t)0x00432940;

typedef void (__cdecl* Scr_AddUndefined_t)(scriptInstance_t inst);
static Scr_AddUndefined_t Scr_AddUndefined = (Scr_AddUndefined_t)0x004A2A20;

//
// Misc functions
//
typedef void (__cdecl* Scr_Error_t)(const char *error, bool force_terminal);
static Scr_Error_t Scr_Error = (Scr_Error_t)0x00644900;

typedef int (__cdecl* Scr_GetType_t)(unsigned int index, scriptInstance_t inst);
static Scr_GetType_t Scr_GetType = (Scr_GetType_t)0x0043FAD0;

////////////////////////////////////////////////

typedef void(__cdecl *Scr_Method_t)(scr_entref_t);
typedef void(__cdecl *Scr_Function_t)(void);

typedef Scr_Method_t(__cdecl* Scr_GetMethod_t)(const char** pName);
typedef Scr_Method_t(__cdecl* Scr_GetMethodEx_t)(const char** pName, int* type);

typedef Scr_Function_t(__cdecl* Scr_GetFunction_t)(const char** pName);
typedef Scr_Function_t(__cdecl* Scr_GetFunctionEx_t)(const char** pName, int* type);

static Scr_GetMethod_t Actor_GetMethod = (Scr_GetMethod_t)0x0051AB90;
static Scr_GetMethod_t Sentient_GetMethod = (Scr_GetMethod_t)0x00546810;
static Scr_GetMethod_t Player_GetMethod = (Scr_GetMethod_t)0x004B3AE0;
static Scr_GetMethod_t ScriptEnt_GetMethod = (Scr_GetMethod_t)0x00562E00;
static Scr_GetMethod_t ScriptVehicle_GetMethod = (Scr_GetMethod_t)0x00479130;
static Scr_GetMethod_t HudElem_GetMethod = (Scr_GetMethod_t)0x0040FCE0;
static Scr_GetMethodEx_t BuiltIn_GetMethod = (Scr_GetMethodEx_t)0x00807160;

static Scr_GetFunction_t Sentient_GetFunction = (Scr_GetFunction_t)0x0051A180;
static Scr_GetFunctionEx_t BuiltIn_GetFunction = (Scr_GetFunctionEx_t)0x00516310;

Scr_Method_t __cdecl Scr_GetMethod(const char **pName, int *type);
Scr_Function_t __cdecl Scr_GetFunction(const char **pName, int *type);

void Scr_UserStat_Get(void);
void Scr_UserStat_Set(void);

void Scr_UserStat_Get_Method(void);
void Scr_UserStat_Set_Method(void);
