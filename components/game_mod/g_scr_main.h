#pragma once

struct scr_entref_t
{
	unsigned __int16 entnum;
	unsigned __int16 classnum;
	unsigned __int16 client;
};

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
